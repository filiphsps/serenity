/*
 * Copyright (c) 2022, Filiph Sandström <filiph.sandstrom@filfatstudios.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/StringView.h>
#include <LibGfx/AntiAliasingPainter.h>
#include <LibGfx/Bitmap.h>
#include <LibGfx/CharacterBitmap.h>
#include <LibGfx/Painter.h>
#include <LibGfx/Palette.h>
#include <StylePainters/BasicStylePainter/BasicStylePainter.h>

void BasicStylePainter::paint_tab_button(Gfx::Painter&, Gfx::IntRect const&, Palette const&, bool, bool, bool, GUI::TabWidget::TabPosition, bool, bool)
{
    VERIFY_NOT_REACHED();
}

void BasicStylePainter::paint_button(Gfx::Painter&, Gfx::IntRect const&, Palette const&, Gfx::ButtonStyle, bool, bool, bool, bool, bool, bool)
{
    VERIFY_NOT_REACHED();
}

void BasicStylePainter::paint_frame(Gfx::Painter&, Gfx::IntRect const&, Palette const&, Gfx::FrameShape, Gfx::FrameShadow, int, bool)
{
    VERIFY_NOT_REACHED();
}

void BasicStylePainter::paint_window_frame(Gfx::Painter&, Gfx::IntRect const&, Palette const&)
{
    VERIFY_NOT_REACHED();
}

void BasicStylePainter::paint_progressbar(Gfx::Painter&, Gfx::IntRect const&, Palette const&, int, int, int, StringView, Orientation)
{
    VERIFY_NOT_REACHED();
}

void BasicStylePainter::paint_radio_button(Gfx::Painter&, Gfx::IntRect const&, Palette const&, bool, bool)
{
    VERIFY_NOT_REACHED();
}

void BasicStylePainter::paint_check_box(Gfx::Painter&, Gfx::IntRect const&, Palette const&, bool, bool, bool)
{
    VERIFY_NOT_REACHED();
}

void BasicStylePainter::paint_transparency_grid(Gfx::Painter&, Gfx::IntRect const&, Palette const&)
{
    VERIFY_NOT_REACHED();
}

void BasicStylePainter::paint_simple_rect_shadow(Gfx::Painter&, Gfx::IntRect const&, Gfx::Bitmap const&, bool, bool)
{
    VERIFY_NOT_REACHED();
}
