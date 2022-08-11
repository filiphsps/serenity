/*
 * Copyright (c) 2022, Filiph Sandström <filiph.sandstrom@filfatstudios.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Forward.h>
#include <LibGfx/Forward.h>
#include <LibGfx/StylePainter.h>

class BasicStylePainter : public Gfx::BaseStylePainter {
public:
    virtual void paint_button(Gfx::Painter&, Gfx::IntRect const&, Palette const&, Gfx::ButtonStyle, bool pressed, bool hovered = false, bool checked = false, bool enabled = true, bool focused = false, bool default_button = false) override;
    virtual void paint_tab_button(Gfx::Painter&, Gfx::IntRect const&, Palette const&, bool active, bool hovered, bool enabled, GUI::TabWidget::TabPosition position, bool in_active_window, bool accented) override;
    virtual void paint_frame(Gfx::Painter&, Gfx::IntRect const&, Palette const&, Gfx::FrameShape, Gfx::FrameShadow, int thickness, bool skip_vertical_lines = false) override;
    virtual void paint_window_frame(Gfx::Painter&, Gfx::IntRect const&, Palette const&) override;
    virtual void paint_progressbar(Gfx::Painter&, Gfx::IntRect const&, Palette const&, int min, int max, int value, StringView text, Orientation = Orientation::Horizontal) override;
    virtual void paint_radio_button(Gfx::Painter&, Gfx::IntRect const&, Palette const&, bool is_checked, bool is_being_pressed) override;
    virtual void paint_check_box(Gfx::Painter&, Gfx::IntRect const&, Palette const&, bool is_enabled, bool is_checked, bool is_being_pressed) override;
    virtual void paint_transparency_grid(Gfx::Painter&, Gfx::IntRect const&, Palette const&) override;
    virtual void paint_simple_rect_shadow(Gfx::Painter&, Gfx::IntRect const&, Gfx::Bitmap const& shadow_bitmap, bool shadow_includes_frame, bool fill_content) override;
};
