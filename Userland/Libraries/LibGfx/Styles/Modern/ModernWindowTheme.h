/*
 * Copyright (c) 2020, Andreas Kling <kling@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibGfx/Color.h>
#include <LibGfx/WindowTheme.h>

namespace Gfx {

class ModernWindowTheme final : public WindowTheme {
public:
    ModernWindowTheme() = default;
    virtual ~ModernWindowTheme() override = default;

    virtual void paint_normal_frame(Painter& painter, WindowState window_state, WindowMode window_mode, IntRect const& window_rect, StringView window_title, Bitmap const& icon, Palette const& palette, IntRect const& leftmost_button_rect, int menu_row_count, bool window_modified) const override;
    virtual void paint_notification_frame(Painter&, WindowMode, IntRect const& window_rect, Palette const&, IntRect const& close_button_rect) const override;

    virtual int titlebar_height(WindowType, WindowMode, Palette const&) const override;
    virtual IntRect titlebar_rect(WindowType, WindowMode, IntRect const& window_rect, Palette const&) const override;
    virtual IntRect titlebar_icon_rect(WindowType, WindowMode, IntRect const& window_rect, Palette const&) const override;
    virtual IntRect titlebar_text_rect(WindowType, WindowMode, IntRect const& window_rect, Palette const&) const override;

    virtual IntRect menubar_rect(WindowType, WindowMode, IntRect const& window_rect, Palette const&, int menu_row_count) const override;

    virtual IntRect frame_rect_for_window(WindowType, WindowMode, IntRect const& window_rect, Palette const&, int menu_row_count) const override;

    virtual Vector<IntRect> layout_buttons(WindowType, WindowMode, IntRect const& window_rect, Palette const&, size_t buttons) const override;
    virtual bool is_simple_rect_frame() const override { return true; }
    virtual bool frame_uses_alpha(WindowState, Palette const&) const override
    {
        return true;
    }
    virtual float frame_alpha_hit_threshold(WindowState) const override { return 1.0f; }
};

}
