/*
 * Copyright (c) 2023, Filiph Sandström <filiph.sandstrom@filfatstudios.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/JsonObject.h>
#include <AK/JsonValue.h>
#include <AK/String.h>
#include <LibFileSystem/FileSystem.h>
#include <LibGUI/FileSystemMetadata.h>
#include <LibGUI/Icon.h>

namespace GUI {

FileSystemMetadata::FileSystemMetadata()
    : m_impl(FileSystemMetadataImpl::create())
{
}
FileSystemMetadata::FileSystemMetadata(FileSystemMetadataImpl const& impl)
    : m_impl(impl)
{
}

ErrorOr<void> FileSystemMetadataImpl::try_set_icon(StringView name)
{
    if (name.is_empty())
        return {};

    auto icon_or_error = Icon::try_create_default_icon(name);
    if (icon_or_error.is_error())
        return {};

    m_icon = icon_or_error.release_value();
    m_icon_name = name;

    return {};
}

ErrorOr<FileSystemMetadata> FileSystemMetadata::try_create(StringView path)
{
    auto metadata_path = MUST(String::formatted("{}/.metadata.json"sv, path));
    if (!FileSystem::exists(metadata_path))
        return FileSystemMetadata();

    auto file = TRY(Core::File::open(metadata_path, Core::File::OpenMode::Read));
    auto file_contents = TRY(file->read_until_eof());
    auto json = TRY(JsonValue::from_string(file_contents));

    if (!json.is_object()) {
        warnln("Invalid metadata found for {}", path);
        return FileSystemMetadata();
    }

    auto metadata = json.as_object();

    auto impl = FileSystemMetadataImpl::create();
    TRY(impl->try_set_icon(metadata.get_deprecated_string("icon"sv).value_or("")));

    return FileSystemMetadata(impl);
}

}
