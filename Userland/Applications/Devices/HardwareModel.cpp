/*
 * Copyright (c) 2018-2021, Andreas Kling <kling@serenityos.org>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "HardwareModel.h"
#include <AK/JsonObject.h>
#include <AK/JsonValue.h>
#include <AK/NonnullRefPtr.h>
#include <AK/NumberFormat.h>
#include <LibCore/DirIterator.h>
#include <LibCore/File.h>
#include <LibCore/ProcessStatisticsReader.h>
#include <LibCore/Stream.h>
#include <LibGUI/FileIconProvider.h>
#include <LibGUI/Icon.h>
#include <LibGUI/ModelIndex.h>
#include <LibGUI/ModelRole.h>
#include <LibPCIDB/Database.h>
#include <unistd.h>

static HardwareModel* s_the;

HardwareModel& HardwareModel::the()
{
    VERIFY(s_the);
    return *s_the;
}

HardwareModel::HardwareModel()
{
    VERIFY(!s_the);
    s_the = this;

    // TODO
    m_db = PCIDB::Database::open();
}

int HardwareModel::row_count(GUI::ModelIndex const& index) const
{
    if (!index.is_valid())
        return m_categories.size();

    if (index.parent().is_valid())
        return 0;

    auto const& device = *static_cast<Device const*>(index.internal_data());
    if (device.is_label && device.category.devices.size() > 1)
        return device.category.devices.size() - 1;
    return 0;
}

int HardwareModel::column_count(GUI::ModelIndex const&) const
{
    return Column::__Count;
}

String HardwareModel::column_name(int column) const
{
    switch (column) {
    case Column::Icon:
        return "";
    case Column::Id:
        return "Id";
    case Column::Name:
        return "Name";
    case Column::Vendor:
        return "Vendor";
    default:
        VERIFY_NOT_REACHED();
    }
}

GUI::Variant HardwareModel::data(GUI::ModelIndex const& index, GUI::ModelRole role) const
{
    VERIFY(is_within_range(index));

    if (role == GUI::ModelRole::TextAlignment) {
        switch (index.column()) {
        case Column::Icon:
        case Column::Id:
        case Column::Name:
        case Column::Vendor:
            return Gfx::TextAlignment::CenterLeft;
        default:
            VERIFY_NOT_REACHED();
        }
    }

    auto const& device = *static_cast<Device const*>(index.internal_data());

    if (role == GUI::ModelRole::Sort) {
        switch (index.column()) {
        case Column::Icon:
            return 0;
        case Column::Id:
            return device.id;
        case Column::Name:
            return device.name;
        case Column::Vendor:
            return device.vendor;
        }
        VERIFY_NOT_REACHED();
    }

    if (role == GUI::ModelRole::Display) {
        switch (index.column()) {
        case Column::Icon:
            return icon_for(device);
        case Column::Id:
            return device.id;
        case Column::Name:
            return device.name;
        case Column::Vendor:
            return device.vendor;
        }
    }

    if (role == GUI::ModelRole::Icon)
        return icon_for(device);

    return {};
}

GUI::Icon HardwareModel::icon_for(Device const& device) const
{
    auto category_name = device.category.label().value()->name;
    if (category_name.contains("display"sv, AK::CaseSensitivity::CaseInsensitive))
        return GUI::Icon::default_icon("app-display-settings"sv);

    if (category_name.contains("network"sv, AK::CaseSensitivity::CaseInsensitive))
        return GUI::Icon::default_icon("network"sv);

    if (category_name.contains("storage"sv, AK::CaseSensitivity::CaseInsensitive))
        return GUI::Icon::default_icon("hard-disk"sv);

    return GUI::Icon::default_icon("gear"sv);
}

GUI::ModelIndex HardwareModel::index(int row, int column, GUI::ModelIndex const& parent) const
{
    if (row < 0 || column < 0)
        return {};

    if (!parent.is_valid()) {
        if (row >= static_cast<int>(m_categories.size()))
            return {};

        auto device = m_categories[row].label();
        if (!device.has_value())
            return {};

        return create_index(row, column, device.release_value().ptr());
    }

    auto const& parent_device = *static_cast<Device const*>(parent.internal_data());
    auto const& category = parent_device.category;
    if (row >= static_cast<int>(category.devices.size()))
        return {};

    return create_index(row, column, &category.devices_without_label(row));
}

GUI::ModelIndex HardwareModel::parent_index(GUI::ModelIndex const& index) const
{
    if (!index.is_valid())
        return {};

    auto const& device = *static_cast<Device*>(index.internal_data());
    if (device.is_label)
        return {};

    auto const& parent = device.category;
    if (!parent.label().has_value())
        return {};

    return create_index(m_categories.find_first_index(parent).release_value(), index.column(), parent.label().value().ptr());
}

int HardwareModel::device_model_row(Device const& device) const
{
    auto const& category = device.category;
    // A process's main thread uses the global process index.
    if (category.id == device.category.id)
        return m_categories.find_first_index(category).value_or(0);

    return category.devices.find_first_index(device).value_or(0);
}

Vector<GUI::ModelIndex> HardwareModel::matches(StringView searching, unsigned flags, GUI::ModelIndex const&)
{
    Vector<GUI::ModelIndex> found_indices;

    for (auto const& device : m_devices) {
        if (string_matches(device.value->name, searching, flags)) {
            auto did_row = device_model_row(device.value);

            found_indices.append(create_index(did_row, Column::Name, reinterpret_cast<void const*>(device.value.ptr())));
            if (flags & FirstMatchOnly)
                break;
        }
    }

    return found_indices;
}

static u32 read_hex_string_from_bytebuffer(ByteBuffer const& buf)
{
    // FIXME: Propagate errors.
    return AK::StringUtils::convert_to_uint_from_hex(
        String(MUST(buf.slice(2, buf.size() - 2)).bytes()))
        .release_value();
}

NonnullOwnPtr<HardwareModel::Category>* HardwareModel::get_or_create_category(String id, String title)
{
    auto maybe_category = m_categories.find_if([&](auto const& category) { return category->id == id; });
    if (maybe_category.index() != m_categories.end().index()) {
        return &m_categories.ptr_at(maybe_category.index());
    }

    m_categories.append(make<Category>(id));
    NonnullOwnPtr<Category>* category = &m_categories.ptr_at(m_categories.size() - 1);

    auto device = m_devices.ensure(id, [&] { return make_ref_counted<Device>(**category); });
    device->is_label = true;
    device->name = title;
    if (auto maybe_device_index = (*category)->devices.find_first_index(device); maybe_device_index.has_value()) {
        (*category)->devices.ptr_at(maybe_device_index.value()) = device;
    } else {
        (*category)->devices.append(device);
    }
    return category;
}

void HardwareModel::update()
{
    auto previous_count = m_devices.size();

    Core::DirIterator di("/sys/bus/pci/", Core::DirIterator::SkipParentAndBaseDir);
    if (di.has_error()) {
        warnln("Failed to open /sys/bus/pci - {}", di.error());
        return;
    }

    while (di.has_next()) {
        auto dir = di.next_path();

        auto vendor_id_filename = String::formatted("/sys/bus/pci/{}/vendor", dir);
        auto vendor_id_file = Core::Stream::File::open(vendor_id_filename, Core::Stream::OpenMode::Read);
        if (vendor_id_file.is_error()) {
            dbgln("Error: Could not open {}: {}", vendor_id_filename, vendor_id_file.error());
            continue;
        }
        auto device_id_filename = String::formatted("/sys/bus/pci/{}/device_id", dir);
        auto device_id_file = Core::Stream::File::open(device_id_filename, Core::Stream::OpenMode::Read);
        if (device_id_file.is_error()) {
            dbgln("Error: Could not open {}: {}", device_id_filename, device_id_file.error());
            continue;
        }
        auto class_id_filename = String::formatted("/sys/bus/pci/{}/class", dir);
        auto class_id_file = Core::Stream::File::open(class_id_filename, Core::Stream::OpenMode::Read);
        if (class_id_file.is_error()) {
            dbgln("Error: Could not open {}: {}", class_id_filename, class_id_file.error());
            continue;
        }

        auto vendor_id_contents = vendor_id_file.value()->read_all();
        if (vendor_id_contents.is_error()) {
            dbgln("Error: Could not read {}: {}", vendor_id_filename, vendor_id_contents.error());
            continue;
        }
        u32 vendor_id = read_hex_string_from_bytebuffer(vendor_id_contents.value());
        auto device_id_contents = device_id_file.value()->read_all();
        if (device_id_contents.is_error()) {
            dbgln("Error: Could not read {}: {}", device_id_filename, device_id_contents.error());
            continue;
        }
        u32 device_id = read_hex_string_from_bytebuffer(device_id_contents.value());
        auto class_id_contents = class_id_file.value()->read_all();
        if (class_id_contents.is_error()) {
            dbgln("Error: Could not read {}: {}", class_id_filename, class_id_contents.error());
            continue;
        }
        u32 class_id = read_hex_string_from_bytebuffer(class_id_contents.value());

        String vendor_name;
        String device_name;
        String class_name;

        if (m_db) {
            vendor_name = m_db->get_vendor(vendor_id);
            device_name = m_db->get_device(vendor_id, device_id);
            class_name = m_db->get_class(class_id);
        }

        auto category = get_or_create_category(String::formatted("{}", class_id), class_name);
        auto device = m_devices.ensure(dir, [&] { return make_ref_counted<Device>(**category); });
        device->id = dir;
        device->name = device_name;
        device->vendor = vendor_name;

        if (auto maybe_device_index = (*category)->devices.find_first_index(device); maybe_device_index.has_value()) {
            (*category)->devices.ptr_at(maybe_device_index.value()) = device;
        } else {
            (*category)->devices.append(device);
        }
    }

    did_update(previous_count == m_devices.size() ? GUI::Model::UpdateFlag::DontInvalidateIndices : GUI::Model::UpdateFlag::InvalidateAllIndices);
}

bool HardwareModel::is_default_column(int index) const
{
    switch (index) {
    case Column::Name:
    case Column::Vendor:
        return true;
    default:
        return false;
    }
}
