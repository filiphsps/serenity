/*
 * Copyright (c) 2018-2020, Andreas Kling <kling@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "NavbarButton.h"
#include "../Taskbar/WindowList.h"
#include <LibGUI/Action.h>
#include <LibGUI/ConnectionToWindowManagerServer.h>
#include <LibGUI/ConnectionToWindowServer.h>
#include <LibGUI/Painter.h>
#include <LibGfx/Font/Font.h>
#include <LibGfx/Palette.h>
#include <LibGfx/StylePainter.h>

NavbarButton::NavbarButton(WindowIdentifier const& identifier)
    : m_identifier(identifier)
{
}

void NavbarButton::context_menu_event(GUI::ContextMenuEvent&)
{
    GUI::ConnectionToWindowManagerServer::the().async_popup_window_menu(
        m_identifier.client_id(),
        m_identifier.window_id(),
        screen_relative_rect().location());
}

void NavbarButton::update_taskbar_rect()
{
    GUI::ConnectionToWindowManagerServer::the().async_set_window_taskbar_rect(
        m_identifier.client_id(),
        m_identifier.window_id(),
        screen_relative_rect());
}

void NavbarButton::clear_taskbar_rect()
{
    GUI::ConnectionToWindowManagerServer::the().async_set_window_taskbar_rect(
        m_identifier.client_id(),
        m_identifier.window_id(),
        {});
}

void NavbarButton::resize_event(GUI::ResizeEvent& event)
{
    update_taskbar_rect();
    return GUI::Button::resize_event(event);
}

static void paint_custom_progressbar(GUI::Painter& painter, Gfx::IntRect const& rect, Palette const& palette, int min, int max, int value)
{
    float range_size = max - min;
    float progress = (value - min) / range_size;
    float progress_width = progress * rect.width();

    Gfx::IntRect progress_rect { rect.x(), rect.y(), (int)progress_width, rect.height() };

    {
        Gfx::PainterStateSaver saver(painter);
        painter.add_clip_rect(progress_rect);

        Color start_color = palette.active_window_border1();
        Color end_color = palette.active_window_border2();
        painter.fill_rect_with_gradient(rect, start_color, end_color);
    }

    Gfx::IntRect hole_rect { (int)progress_width, 0, (int)(rect.width() - progress_width), rect.height() };
    hole_rect.translate_by(rect.location());
    hole_rect.set_right_without_resize(rect.right());
    Gfx::PainterStateSaver saver(painter);
    painter.add_clip_rect(hole_rect);
}

void NavbarButton::paint_event(GUI::PaintEvent& event)
{
    VERIFY(icon());
    auto& icon = *this->icon();
    auto& window = WindowList::the().ensure_window(m_identifier);

    GUI::Painter painter(*this);
    painter.add_clip_rect(event.rect());

    Gfx::StylePainter::paint_button(painter, rect(), palette(), button_style(), is_being_pressed(), is_hovered(), is_checked(), is_enabled());

    auto content_rect = rect();
    auto icon_location = content_rect.center().translated(-(icon.width() / 2), -(icon.height() / 2));

    if (is_being_pressed() || is_checked()) {
        icon_location.translate_by(1, 1);
    }

    if (window.progress().has_value()) {
        auto adjusted_rect = rect().shrunken(4, 4);
        if (!is_being_pressed() && !is_checked()) {
            adjusted_rect.translate_by(-1, -1);
            adjusted_rect.set_width(adjusted_rect.width() + 1);
            adjusted_rect.set_height(adjusted_rect.height() + 1);
        }
        paint_custom_progressbar(painter, adjusted_rect, palette(), 0, 100, window.progress().value());
    }

    if (is_enabled()) {
        if (is_hovered())
            painter.blit_brightened(icon_location, icon, icon.rect());
        else
            painter.blit(icon_location, icon, icon.rect());
    } else {
        painter.blit_disabled(icon_location, icon, icon.rect(), palette());
    }
}
