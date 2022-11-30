/*
 * Copyright (c) 2018-2021, Andreas Kling <kling@serenityos.org>
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/HashMap.h>
#include <AK/NonnullOwnPtrVector.h>
#include <AK/String.h>
#include <AK/Vector.h>
#include <LibGUI/Icon.h>
#include <LibGUI/Model.h>
#include <LibGUI/ModelIndex.h>
#include <LibPCIDB/Database.h>
#include <sys/types.h>
#include <unistd.h>

class HardwareModel final : public GUI::Model {
public:
    enum Column {
        Icon = 0,
        Id,
        Name,
        Vendor,
        __Count
    };

    static HardwareModel& the();

    static NonnullRefPtr<HardwareModel> create() { return adopt_ref(*new HardwareModel); }
    virtual ~HardwareModel() override = default;

    virtual int tree_column() const override { return Column::Name; }
    virtual int row_count(GUI::ModelIndex const&) const override;
    virtual int column_count(GUI::ModelIndex const&) const override;
    virtual String column_name(int column) const override;
    virtual GUI::Variant data(GUI::ModelIndex const&, GUI::ModelRole) const override;
    virtual GUI::ModelIndex index(int row, int column, GUI::ModelIndex const& parent = {}) const override;
    virtual GUI::ModelIndex parent_index(GUI::ModelIndex const&) const override;
    virtual bool is_searchable() const override { return true; }
    virtual Vector<GUI::ModelIndex> matches(StringView, unsigned = MatchesFlag::AllMatching, GUI::ModelIndex const& = GUI::ModelIndex()) override;
    virtual bool is_column_sortable(int column_index) const override { return column_index != Column::Icon; }
    void update();
    bool is_default_column(int index) const;

private:
    HardwareModel();

    struct Category;

    struct Device : public RefCounted<Device> {
        String id { "" };
        String name { "" };
        String vendor { "" };
        bool is_label { false };
        Category& category;

        Device(Category& category)
            : category(category)
        {
        }

        bool operator==(Device const& other) const
        {
            return id == other.id;
        }
    };

    struct Category {
        String id { "" };
        NonnullRefPtrVector<Device> devices;

        Category(String id)
            : id(id)
        {
        }

        bool operator==(Category const& other) const
        {
            return this->id == other.id;
        }

        Optional<NonnullRefPtr<Device>> label() const
        {
            return devices.first_matching([this](auto const device) { return device->is_label; });
        }

        Device const& devices_without_label(size_t index) const
        {
            auto main_device_index = -1;
            for (size_t i = 0; i < devices.size(); ++i) {
                if (devices[i].is_label) {
                    main_device_index = static_cast<int>(i);
                    break;
                }
            }
            VERIFY(main_device_index >= 0);
            if (index >= static_cast<size_t>(main_device_index))
                return devices[index + 1];
            return devices[index];
        }
    };

    GUI::Icon icon_for(Device const& device) const;

    int device_model_row(Device const& device) const;
    NonnullOwnPtr<Category>* get_or_create_category(String id, String title);

    HashMap<String, NonnullRefPtr<Device>> m_devices;
    NonnullOwnPtrVector<Category> m_categories;
    RefPtr<PCIDB::Database> m_db;
};
