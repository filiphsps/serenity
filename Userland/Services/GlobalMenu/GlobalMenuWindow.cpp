/*
 * Copyright (c) 2018-2021, Andreas Kling <kling@serenityos.org>
 * Copyright (c) 2021, Spencer Dixon <spencercdixon@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "GlobalMenuWindow.h"
#include <AK/Debug.h>
#include <LibGUI/BoxLayout.h>
#include <LibGUI/Button.h>
#include <LibGUI/ConnectionToWindowManagerServer.h>
#include <LibGUI/ConnectionToWindowServer.h>
#include <LibGUI/Desktop.h>
#include <LibGUI/Frame.h>
#include <LibGUI/Icon.h>
#include <LibGUI/Menu.h>
#include <LibGUI/Painter.h>
#include <LibGUI/Window.h>
#include <LibGfx/Palette.h>
#include <serenity.h>
#include <stdio.h>

class MenuWidget final : public GUI::Widget {
    C_OBJECT(MenuWidget);

public:
    virtual ~MenuWidget() override = default;

private:
    MenuWidget() = default;

    virtual void paint_event(GUI::PaintEvent& /*event*/) override
    {
        /*GUI::Painter painter(*this);
        painter.add_clip_rect(event.rect());
        painter.fill_rect(rect(), palette().button());
        painter.draw_line({ 0, height() - 1 }, { width() - 1, height() }, palette().threed_highlight());*/
    }
};

GlobalMenuWindow::GlobalMenuWindow()
{
    set_window_type(GUI::WindowType::Taskbar);
    set_title("Global Menu");

    on_screen_rects_change(GUI::Desktop::the().rects(), GUI::Desktop::the().main_screen_index());

    auto& main_widget = set_main_widget<MenuWidget>();
    main_widget.set_layout<GUI::HorizontalBoxLayout>();
    main_widget.layout()->set_margins({ 2, 2, 2, 2 });

    m_menu_area_container = main_widget.add<GUI::Frame>();
    m_menu_area_container->set_frame_thickness(0);
    m_menu_area_container->set_frame_shape(Gfx::FrameShape::Box);
    m_menu_area_container->set_frame_shadow(Gfx::FrameShadow::Plain);
}

void GlobalMenuWindow::config_string_did_change(DeprecatedString const&, DeprecatedString const&, DeprecatedString const&, DeprecatedString const&)
{
}

void GlobalMenuWindow::on_screen_rects_change(Vector<Gfx::IntRect, 4> const& rects, size_t main_screen_index)
{
    auto const& rect = rects[main_screen_index];
    Gfx::IntRect new_rect { rect.x(), 0, rect.width(), menu_height() };
    set_rect(new_rect);
    update_menu_area();
}

void GlobalMenuWindow::update_menu_area()
{
    if (!main_widget())
        return;
    main_widget()->do_layout();

    auto new_rect = Gfx::IntRect({ 0, 0 }, m_menu_area_size);
    new_rect.center_vertically_within(m_menu_area_container->screen_relative_rect());
    dbgln("update_menu_area: x: {}, y: {}", new_rect.x(), new_rect.y());

    GUI::ConnectionToWindowManagerServer::the().async_set_menu_area_position(new_rect.location());
}

void GlobalMenuWindow::event(Core::Event& event)
{
    switch (event.type()) {
    }
    Window::event(event);
}

void GlobalMenuWindow::wm_event(GUI::WMEvent& event)
{
    WindowIdentifier identifier { event.client_id(), event.window_id() };
    switch (event.type()) {
    case GUI::Event::WM_WindowRemoved: {
        break;
    }
    case GUI::Event::WM_WindowRectChanged: {
        break;
    }
    case GUI::Event::WM_WindowStateChanged: {
        break;
    }
    case GUI::Event::WM_WindowIconBitmapChanged: {
        break;
    }
    case GUI::Event::WM_MenuAreaSizeChanged: {
        auto& changed_event = static_cast<GUI::WMMenuAreaSizeChangedEvent&>(event);
        m_menu_area_size = changed_event.size();
        m_menu_area_container->set_fixed_size(changed_event.size().width() + 8, 21);
        dbgln("GUI::Event::WM_MenuAreaSizeChanged: width: {}, height: {}", changed_event.size().width(), changed_event.size().height());

        update_menu_area();
        break;
    }
    default:
        break;
    }
}

void GlobalMenuWindow::screen_rects_change_event(GUI::ScreenRectsChangeEvent& event)
{
    on_screen_rects_change(event.rects(), event.main_screen_index());
}
