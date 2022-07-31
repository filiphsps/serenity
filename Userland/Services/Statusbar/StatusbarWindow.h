/*
 * Copyright (c) 2018-2020, Andreas Kling <kling@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "../Taskbar/ClockWidget.h"
#include <LibConfig/Listener.h>
#include <LibGUI/Widget.h>
#include <LibGUI/Window.h>
#include <LibGfx/ShareableBitmap.h>
#include <Services/WindowServer/ScreenLayout.h>

class StatusbarWindow final : public GUI::Window
    , public Config::Listener {
    C_OBJECT(StatusbarWindow);

public:
    virtual ~StatusbarWindow() override = default;

    static int statusbar_height() { return 36; }

    virtual void config_string_did_change(String const&, String const&, String const&, String const&) override;

private:
    explicit StatusbarWindow();

    virtual void event(Core::Event&) override;
    virtual void wm_event(GUI::WMEvent&) override;
    virtual void screen_rects_change_event(GUI::ScreenRectsChangeEvent&) override;

    void on_screen_rects_change(Vector<Gfx::IntRect, 4> const&, size_t);

    void update_applet_area();

    RefPtr<GUI::Widget> m_task_button_container;

    Gfx::IntSize m_applet_area_size;
    RefPtr<GUI::Frame> m_applet_area_container;
    RefPtr<Taskbar::ClockWidget> m_clock_widget;
};
