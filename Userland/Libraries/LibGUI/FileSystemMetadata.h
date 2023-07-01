/*
 * Copyright (c) 2023, Filiph Sandström <filiph.sandstrom@filfatstudios.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Forward.h>
#include <AK/NonnullRefPtr.h>
#include <AK/RefCounted.h>
#include <LibGUI/Forward.h>
#include <LibGUI/Icon.h>

namespace GUI {

class FileSystemMetadataImpl : public RefCounted<FileSystemMetadataImpl> {
public:
    static NonnullRefPtr<FileSystemMetadataImpl> create() { return adopt_ref(*new FileSystemMetadataImpl); }
    ~FileSystemMetadataImpl() = default;

    Optional<GUI::Icon> const icon() const { return m_icon; }
    ErrorOr<void> try_set_icon(StringView);

private:
    FileSystemMetadataImpl() = default;
    Optional<GUI::Icon> m_icon;
    StringView m_icon_name;
};

class FileSystemMetadata {
public:
    FileSystemMetadata();
    explicit FileSystemMetadata(FileSystemMetadataImpl const&);

    static ErrorOr<FileSystemMetadata> try_create(StringView);

    Optional<GUI::Icon> const icon() const { return m_impl->icon(); }

    FileSystemMetadataImpl const& impl() const { return *m_impl; }

private:
    NonnullRefPtr<FileSystemMetadataImpl> m_impl;
};

}
