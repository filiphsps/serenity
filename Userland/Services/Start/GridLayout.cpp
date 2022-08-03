/*
 * Copyright (c) 2022, Filiph Sandstrom <filiph.sandstrom@filfatstudios.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "GridLayout.h"

GridLayout::GridLayout()
{
}

GUI::UISize GridLayout::preferred_size() const
{
    return min_size();
}

GUI::UISize GridLayout::min_size() const
{
    int items = 0;
    for (size_t i = 0; i < m_entries.size(); ++i) {
        auto& entry = m_entries[i];
        if (!entry.widget)
            continue;
        if (!entry.widget->is_visible())
            continue;

        items += 1;
    }

    GUI::UIDimension columns { m_item_size * m_columns };
    GUI::UIDimension rows { (m_item_size + margins().top()) * (items / m_columns) };

    return { columns, rows };
}

void GridLayout::run(GUI::Widget& widget)
{
    if (m_entries.is_empty())
        return;

    struct Item {
        GUI::Widget* widget { nullptr };
        int size { 0 };
    };

    Vector<Item, 32> items;
    for (size_t i = 0; i < m_entries.size(); ++i) {
        auto& entry = m_entries[i];
        if (!entry.widget)
            continue;
        if (!entry.widget->is_visible())
            continue;

        auto min_size = entry.widget->effective_min_size().primary_size_for_orientation(Gfx::Orientation::Vertical);
        items.append(Item { entry.widget.ptr(), MUST(min_size.shrink_value()) });
    }

    if (items.is_empty())
        return;

    Gfx::IntRect content_rect = widget.content_rect();
    int current_y = margins().top() + content_rect.y();
    int rows = items.size() / columns();
    int id = 0;

    for (int r = 0; r < rows; r++) {
        int current_x = margins().left() + content_rect.x();

        for (int c = 0; c < columns(); c++) {
            auto& item = items[id];
            if (!item.widget) {
                continue;
                id += 1;
            }

            Gfx::IntRect rect { current_x, current_y, 0, 0 };
            rect.set_primary_size_for_orientation(Gfx::Orientation::Vertical, item.size);
            rect.set_secondary_size_for_orientation(Gfx::Orientation::Vertical, item.size);

            item.widget->set_relative_rect(rect);

            if (c == columns() - 1) {
                current_y += rect.height() + spacing();
            }

            current_x += margins().left() + item.size;
            id += 1;
        }
    }
}
