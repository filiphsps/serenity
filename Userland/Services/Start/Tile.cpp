/*
 * Copyright (c) 2022, Filiph Sandstrom <filiph.sandstrom@filfatstudios.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "Tile.h"
#include <AK/Random.h>
#include <LibGUI/Label.h>
#include <LibGUI/Icon.h>
#include <LibGUI/Painter.h>
#include <LibGfx/Palette.h>
#include <LibGfx/StylePainter.h>

Tile::Tile()
{
    m_animation_start = get_random_uniform(60 * 8) + animation_idle();
    start_timer(16);
}

void Tile::tick()
{
    m_tick += 1;

    if (!m_animation_started && m_tick >= m_animation_start) {
        m_animation_started = true;
        m_tick = 0;
        return;
    }

    if (!m_animation_started || m_tick < m_animation_start)
        return;

    if (m_tick > (animation_idle() * 2) + (animation_duration() * 2))
        m_tick = 0;
    
    invalidate_layout();
}

void Tile::paint_event(GUI::PaintEvent& event)
{
    GUI::Painter painter(*this);
    painter.add_clip_rect(event.rect());

    Gfx::StylePainter::paint_button(painter, rect(), palette(), button_style(), is_being_pressed(), is_hovered(), is_checked(), true);

    auto content_rect = rect();

    int y_translation = 0;
    if (m_animation_enabled && m_tick >= animation_idle()) {
        const int h = height() - 5;
        int ticks_per_pixel = 1.0 * animation_duration() / h;
        if (ticks_per_pixel <= 0)
            ticks_per_pixel = 1;

        if (m_tick >= animation_idle() * 2 + animation_duration()) {
            // Content to icon
            y_translation = (m_tick - (animation_idle() * 2 + animation_duration())) / ticks_per_pixel;
            
            if (y_translation > h)
                y_translation = h;

            content_rect.translate_by(0, -h + y_translation);
        } else if (m_tick >= animation_idle() + animation_duration()) {
            // Idle at content
            content_rect.translate_by(0, -h);
        } else {
            // Icon to content
            y_translation = (m_tick - animation_idle()) / ticks_per_pixel;
            if (y_translation > h)
                y_translation = h;
                
            content_rect.translate_by(0, -y_translation);
        }
    }

    if (m_animation_enabled && m_tick >= animation_idle()) {
        // Show tile content
        auto text_location = Gfx::Rect(rect()).shrunken(6, 6);

        if (m_tick >= animation_idle() + animation_duration())
            text_location.translate_by(0, text_location.top() + y_translation);
        else
            text_location.translate_by(0, text_location.bottom() - y_translation - 6);

        painter.draw_text(text_location, "Hello world from tile content"sv, this->font(), Gfx::TextAlignment::Center, palette().color(foreground_role()), Gfx::TextElision::Right, Gfx::TextWrapping::Wrap);
    }

    // Show tile icon+label
    auto icon_location = content_rect.center().translated(-(icon()->width() / 2), -(icon()->height() / 2));
    painter.blit(icon_location, *icon(), icon()->rect());

    auto text_location = Gfx::Rect(content_rect.x() + 6, content_rect.y() - 6, width(), height());
    paint_text(painter, text_location, this->font(), Gfx::TextAlignment::BottomLeft);
}
