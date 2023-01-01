/*
 * Copyright (c) 2023, Filiph Sandström <filiph.sandstrom@filfatstudios.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibGfx/Bitmap.h>
#include <LibGfx/Font/FontDatabase.h>
#include <LibGfx/Painter.h>
#include <LibGfx/Palette.h>
#include <LibGfx/StylePainter.h>
#include <LibGfx/Styles/Modern/ModernWindowTheme.h>

namespace Gfx {

static constexpr int menubar_height = 20;
static constexpr int border_thickness = 2;
static constexpr int window_titlebar_height = 24;

Gfx::IntRect ModernWindowTheme::titlebar_icon_rect(WindowType window_type, WindowMode window_mode, IntRect const& window_rect, Palette const& palette) const
{
    if (window_mode == WindowMode::RenderAbove)
        return {};

    auto titlebar_rect = this->titlebar_rect(window_type, window_mode, window_rect, palette);
    Gfx::IntRect icon_rect {
        titlebar_rect.x() + 6,
        titlebar_rect.y(),
        16,
        16,
    };
    icon_rect.center_vertically_within(titlebar_rect);
    return icon_rect;
}

Gfx::IntRect ModernWindowTheme::titlebar_text_rect(WindowType window_type, WindowMode window_mode, IntRect const& window_rect, Palette const& palette) const
{
    auto titlebar_rect = this->titlebar_rect(window_type, window_mode, window_rect, palette);
    auto titlebar_icon_rect = this->titlebar_icon_rect(window_type, window_mode, window_rect, palette);
    return {
        titlebar_rect.x() + 6 + (titlebar_icon_rect.is_empty() ? 0 : (titlebar_icon_rect.width() + 6)),
        titlebar_rect.y(),
        titlebar_rect.width() - 6 - (titlebar_icon_rect.is_empty() ? 0 : (titlebar_icon_rect.width() + 6)),
        titlebar_rect.height()
    };
}

void ModernWindowTheme::paint_normal_frame(Painter& painter, WindowState window_state, WindowMode window_mode, IntRect const& window_rect, StringView window_title, Bitmap const& icon, Palette const& palette, IntRect const& leftmost_button_rect, int menu_row_count, [[maybe_unused]] bool window_modified) const
{
    auto frame_rect = frame_rect_for_window(WindowType::Normal, window_mode, window_rect, palette, menu_row_count);
    frame_rect.set_location({ 0, 0 });
    Gfx::StylePainter::paint_window_frame(painter, frame_rect, palette);

    auto& title_font = FontDatabase::window_title_font();

    auto titlebar_rect = this->titlebar_rect(WindowType::Normal, window_mode, window_rect, palette);
    auto titlebar_icon_rect = this->titlebar_icon_rect(WindowType::Normal, window_mode, window_rect, palette);
    auto titlebar_inner_rect = titlebar_text_rect(WindowType::Normal, window_mode, window_rect, palette);
    auto titlebar_title_rect = titlebar_inner_rect;
    titlebar_title_rect.set_width(title_font.width(window_title));

    if (window_state == WindowState::Active)
        painter.fill_rect(titlebar_rect, Gfx::Color(241, 241, 241));
    else
        painter.fill_rect(titlebar_rect, Gfx::Color(205, 205, 205));

    auto clipped_title_rect = titlebar_title_rect;
    clipped_title_rect.set_width((leftmost_button_rect.left() - 3) - clipped_title_rect.x());
    painter.draw_text(clipped_title_rect.translated(4, 0), window_title, title_font, Gfx::TextAlignment::CenterLeft, Gfx::Color(18, 22, 33), Gfx::TextElision::Right);

    if (window_mode == WindowMode::RenderAbove)
        return;

    painter.draw_scaled_bitmap(titlebar_icon_rect, icon, icon.rect());
}

IntRect ModernWindowTheme::menubar_rect(WindowType window_type, WindowMode, IntRect const& window_rect, Palette const&, int menu_row_count) const
{
    if (window_type != WindowType::Normal)
        return {};
    return { border_thickness, border_thickness - 1 + window_titlebar_height + 2, window_rect.width(), menubar_height * menu_row_count };
}

IntRect ModernWindowTheme::titlebar_rect(WindowType window_type, WindowMode, IntRect const& window_rect, Palette const&) const
{
    auto& title_font = FontDatabase::window_title_font();
    // FIXME: The top of the titlebar doesn't get redrawn properly if this padding is different
    int total_vertical_padding = title_font.glyph_height() - 1;

    if (window_type == WindowType::Notification)
        return { window_rect.width() + 3, total_vertical_padding / 2 - 1, window_titlebar_height, window_rect.height() };
    return { border_thickness, border_thickness, window_rect.width(), window_titlebar_height };
}

void ModernWindowTheme::paint_notification_frame(Painter& painter, WindowMode window_mode, IntRect const& window_rect, Palette const& palette, IntRect const& close_button_rect) const
{
    auto frame_rect = frame_rect_for_window(WindowType::Notification, window_mode, window_rect, palette, 0);
    frame_rect.set_location({ 0, 0 });
    Gfx::StylePainter::paint_window_frame(painter, frame_rect, palette);

    auto titlebar_rect = this->titlebar_rect(WindowType::Notification, window_mode, window_rect, palette);
    painter.fill_rect_with_gradient(Gfx::Orientation::Vertical, titlebar_rect, palette.active_window_border1(), palette.active_window_border2());

    if (palette.active_window_title_stripes().alpha() > 0) {
        int stripe_top = close_button_rect.bottom() + 4;
        int stripe_bottom = window_rect.height() - 3;
        if (stripe_top && stripe_bottom && stripe_top < stripe_bottom) {
            for (int i = 2; i <= palette.window_title_height() - 2; i += 2) {
                painter.draw_line({ titlebar_rect.x() + i, stripe_top }, { titlebar_rect.x() + i, stripe_bottom }, palette.active_window_title_stripes());
            }
        }
    }
}

IntRect ModernWindowTheme::frame_rect_for_window(WindowType window_type, WindowMode, IntRect const& window_rect, Gfx::Palette const&, int menu_row_count) const
{
    switch (window_type) {
    case WindowType::Normal:
        return {
            window_rect.x() - border_thickness,
            window_rect.y() - window_titlebar_height - border_thickness - menu_row_count * menubar_height,
            window_rect.width() + (border_thickness * 2),
            window_rect.height() + (border_thickness * 2) + window_titlebar_height + menu_row_count * menubar_height
        };
    default:
        return window_rect;
    }
}

Vector<IntRect> ModernWindowTheme::layout_buttons(WindowType window_type, WindowMode window_mode, IntRect const& window_rect, Palette const& palette, size_t buttons) const
{
    int window_button_width = palette.window_title_button_width();
    int window_button_height = palette.window_title_button_height();
    int pos = titlebar_text_rect(window_type, window_mode, window_rect, palette).right() - 6;
    Vector<IntRect> button_rects;

    for (size_t i = 0; i < buttons; i++) {
        if (window_type == WindowType::Notification) {
            // The button height & width have to be equal or it leaks out of its area
            Gfx::IntRect rect { 0, pos, window_button_height, window_button_height };
            rect.center_horizontally_within(titlebar_rect(window_type, window_mode, window_rect, palette));
            button_rects.append(rect);
            pos += window_button_height;
        } else {
            pos -= window_button_width;
            Gfx::IntRect rect { pos, 0, window_button_width, window_button_height };
            rect.center_vertically_within(titlebar_text_rect(window_type, window_mode, window_rect, palette));
            button_rects.append(rect);
        }
    }
    return button_rects;
}

int ModernWindowTheme::titlebar_height(WindowType window_type, WindowMode window_mode, Palette const& palette) const
{
    auto& title_font = FontDatabase::window_title_font();
    switch (window_type) {
    case WindowType::Normal:
    case WindowType::Notification: {
        if (window_mode == WindowMode::RenderAbove)
            return max(palette.window_title_height() - 4, title_font.glyph_height() + 4);
        else
            return max(palette.window_title_height(), title_font.glyph_height() + 8);
    }
    default:
        return 0;
    }
}

}
