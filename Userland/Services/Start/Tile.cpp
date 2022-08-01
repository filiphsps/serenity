/*
 * Copyright (c) 2022, Filiph Sandstrom <filiph.sandstrom@filfatstudios.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "Tile.h"
#include <AK/Random.h>
#include <LibCore/DateTime.h>
#include <LibGUI/Label.h>
#include <LibGUI/Icon.h>
#include <LibGUI/Painter.h>
#include <LibGfx/Palette.h>
#include <LibGfx/StylePainter.h>
#include <LibGfx/Font/FontDatabase.h>

Tile::Tile()
{
    m_animation_start = get_random_uniform(60 * 8) + animation_idle();
    start_timer(16);
}

void Tile::tick()
{
    if (!animated())
        return;

    m_tick += 1;

    if (!m_animation_started && m_tick >= m_animation_start) {
        m_animation_started = true;
        m_tick = 0;
        return;
    }

    if (!m_animation_started || m_tick < m_animation_start)
        return;

    if (m_tick > (animation_idle() * 2) + (animation_duration() * 2)) {
        m_tick = 0;
        m_round_trips += 1;
    }
    
    repaint();
}

void Tile::draw_normal_tile(GUI::Painter painter, Gfx::IntRect icon_rect, Gfx::IntRect content_rect, bool draw_content)
{
    if (draw_content)
        painter.draw_text(content_rect, m_content, this->font(), m_content_alignment, palette().color(foreground_role()), Gfx::TextElision::Right, Gfx::TextWrapping::Wrap);

    auto icon_location = icon_rect.center().translated(-(icon()->width() / 2), -(icon()->height() / 2));
    painter.blit(icon_location, *icon(), icon()->rect());
}

void Tile::draw_date_tile(GUI::Painter painter)
{
    painter.draw_text(rect().translated(0, this->font().x_height()), Core::DateTime::now().to_string("%A"sv), this->font(), Gfx::TextAlignment::Center, palette().color(foreground_role()), Gfx::TextElision::Right, Gfx::TextWrapping::Wrap);
    painter.draw_text(rect().translated(0, -(this->font().x_height())), Core::DateTime::now().to_string("%e"sv), this->font().bold_variant(), Gfx::TextAlignment::Center, palette().color(foreground_role()), Gfx::TextElision::Right, Gfx::TextWrapping::Wrap);
}

void Tile::draw_tile(GUI::Painter painter, Tile::Animation animation_state)
{
    switch(m_kind) {
        case TileKind::Date:
            draw_date_tile(painter);
            break;
        case TileKind::Normal:
        default:
            draw_normal_tile(painter, animation_state.icon_rect, animation_state.content_rect, animation_state.draw_content);
            break;
    }

    if(m_branding == TileBranding::Label) {
        auto text_location = rect().translated(6, -6);
        if (m_content_alignment == Gfx::TextAlignment::BottomLeft)
            text_location = Gfx::Rect(animation_state.icon_rect).translated(6, -6);

        paint_text(painter, text_location, this->font(), Gfx::TextAlignment::BottomLeft);
    }
}

Tile::Animation Tile::process_animation()
{
    auto icon_rect = rect();
    auto content_rect = Gfx::Rect(rect()).shrunken(12, 12).translated(0, -3);

    int y_translation = 0;
    if (animated() && m_tick >= animation_idle()) {
        const int tick = m_tick - animation_idle();

        if (tick >= animation_idle()+ animation_duration()) {
            // Content to icon
            y_translation = content_rect.height() - content_rect.height() * pow(0.85, tick - (animation_idle() + animation_duration()));
            if (y_translation + 3 > content_rect.height())
                y_translation = content_rect.height();

            icon_rect.translate_by(0, -content_rect.height() + y_translation);
        } else if (tick >= animation_duration()) {
            // Idle at content
            icon_rect.translate_by(0, -content_rect.height());
        } else {
            // Icon to content
            y_translation = content_rect.height() - content_rect.height() * pow(0.85, tick);
            if (y_translation > content_rect.height())
                y_translation = content_rect.height();

            icon_rect.translate_by(0, -y_translation);
        }
    }

    bool show_content = false;
    if (animated() && m_tick >= animation_idle()) {
        if (m_tick >= animation_idle() + animation_duration())
            content_rect.translate_by(0, content_rect.top() + y_translation);
        else
            content_rect.translate_by(0, content_rect.bottom() - y_translation);

        show_content = true;
    }

    return {
        icon_rect,
        content_rect,
        show_content
    };
}

void Tile::paint_event(GUI::PaintEvent& event)
{
    GUI::Painter painter(*this);
    painter.add_clip_rect(event.rect());

    Gfx::StylePainter::paint_tile(painter, rect(), palette(), is_being_pressed(), is_hovered());

    auto animation_state = process_animation();
    draw_tile(painter, animation_state);
}
