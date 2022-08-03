/*
 * Copyright (c) 2022, Filiph Sandstrom <filiph.sandstrom@filfatstudios.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/String.h>
#include <LibGUI/Layout.h>
#include <LibGUI/UIDimensions.h>
#include <LibGUI/Widget.h>

class GridLayout final : public GUI::Layout {
    C_OBJECT(GridLayout);

public:

    virtual void run(GUI::Widget&) override;
    virtual GUI::UISize preferred_size() const override;
    virtual GUI::UISize min_size() const override;

    int item_size() { return m_item_size; }
    void set_item_size(int item_size) { m_item_size = item_size; }

    int columns() { return m_columns; }
    void set_columns(int columns) { m_columns = columns; }

private:
    explicit GridLayout();
    virtual ~GridLayout() override = default;

    int m_item_size;
    int m_columns;
};
