/*
 * Copyright (c) 2018-2021, Andreas Kling <kling@serenityos.org>
 * Copyright (c) 2021, Spencer Dixon <spencercdixon@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "StatusbarWindow.h"
#include "../Taskbar/ClockWidget.h"
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
#include <LibGfx/Font/FontDatabase.h>
#include <LibGfx/Palette.h>
#include <serenity.h>
#include <stdio.h>

class StatusbarWidget final : public GUI::Widget {
    C_OBJECT(StatusbarWidget);

public:
    virtual ~StatusbarWidget() override = default;

    virtual void paint_event(GUI::PaintEvent& event) override
    {
        GUI::Painter painter(*this);
        painter.add_clip_rect(event.rect());
        painter.fill_rect(rect(), palette().button());
        painter.draw_line({ 0, height() - 1 }, { width() - 1, height() - 1 }, palette().threed_highlight());
    }

private:
    StatusbarWidget() = default;
};

StatusbarWindow::StatusbarWindow()
{
    set_window_type(GUI::WindowType::Taskbar);
    set_title("Statusbar");

    on_screen_rects_change(GUI::Desktop::the().rects(), GUI::Desktop::the().main_screen_index());

    auto& main_widget = set_main_widget<StatusbarWidget>();
    main_widget.set_layout<GUI::HorizontalBoxLayout>();
    main_widget.layout()->set_margins({ 3, 3, 0, 3 });

    m_clock_widget = main_widget.add<Taskbar::ClockWidget>();

    m_task_button_container = main_widget.add<GUI::Widget>();
    m_task_button_container->set_layout<GUI::HorizontalBoxLayout>();
    m_task_button_container->layout()->set_spacing(3);

    m_applet_area_container = main_widget.add<GUI::Frame>();
    m_applet_area_container->set_frame_thickness(1);
    m_applet_area_container->set_frame_shape(Gfx::FrameShape::Box);
    m_applet_area_container->set_frame_shadow(Gfx::FrameShadow::Sunken);
}

void StatusbarWindow::config_string_did_change(String const& domain, String const& group, String const& key, String const& value)
{
    VERIFY(domain == "Taskbar");
    if (group == "Clock" && key == "TimeFormat") {
        m_clock_widget->update_format(value);
    }
}

void StatusbarWindow::on_screen_rects_change(Vector<Gfx::IntRect, 4> const& rects, size_t main_screen_index)
{
    auto const& rect = rects[main_screen_index];
    Gfx::IntRect new_rect { rect.x(), rect.top(), rect.width(), statusbar_height() };
    set_rect(new_rect);
}

void StatusbarWindow::update_applet_area()
{
    // NOTE: Widget layout is normally lazy, but here we have to force it right away so we can tell
    //       WindowServer where to place the applet area window.
    if (!main_widget())
        return;
    main_widget()->do_layout();
    auto new_rect = Gfx::IntRect({}, m_applet_area_size).centered_within(m_applet_area_container->screen_relative_rect());
    GUI::ConnectionToWindowManagerServer::the().async_set_applet_area_position(new_rect.location());
}

void StatusbarWindow::event(Core::Event& event)
{
    Window::event(event);
}

void StatusbarWindow::wm_event(GUI::WMEvent& event)
{
    WindowIdentifier identifier { event.client_id(), event.window_id() };
    switch (event.type()) {
    case GUI::Event::WM_WindowRectChanged: {
        if constexpr (EVENT_DEBUG) {
            auto& changed_event = static_cast<GUI::WMWindowRectChangedEvent&>(event);
            dbgln("WM_WindowRectChanged: client_id={}, window_id={}, rect={}",
                changed_event.client_id(),
                changed_event.window_id(),
                changed_event.rect());
        }
        break;
    }
    case GUI::Event::WM_AppletAreaSizeChanged: {
        auto& changed_event = static_cast<GUI::WMAppletAreaSizeChangedEvent&>(event);
        m_applet_area_size = changed_event.size();
        m_applet_area_container->set_fixed_size(changed_event.size().width() + 8, 21);
        update_applet_area();
        break;
    }
    default:
        break;
    }
}

void StatusbarWindow::screen_rects_change_event(GUI::ScreenRectsChangeEvent& event)
{
    on_screen_rects_change(event.rects(), event.main_screen_index());
}
