/*
 * Copyright (c) 2018-2020, Andreas Kling <kling@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "../Taskbar/WindowList.h"
#include <LibDesktop/AppFile.h>
#include <LibGUI/Widget.h>
#include <LibGUI/Window.h>
#include <LibGfx/ShareableBitmap.h>
#include <Services/WindowServer/ScreenLayout.h>

class NavbarWindow final : public GUI::Window {
    C_OBJECT(NavbarWindow);

public:
    virtual ~NavbarWindow() override = default;

    static int navbar_height() { return 74; }
    static int navbar_icon_size() { return 42; }

private:
    explicit NavbarWindow(NonnullRefPtr<GUI::Menu> start_menu);
    static void show_desktop_button_clicked(unsigned);
    static void toggle_show_desktop();
    void set_quick_launch_button_data(GUI::Button&, String const&, NonnullRefPtr<Desktop::AppFile>);
    void on_screen_rects_change(Vector<Gfx::IntRect, 4> const&, size_t);
    NonnullRefPtr<GUI::Button> create_button(WindowIdentifier const&);
    void add_window_button(::Window&, WindowIdentifier const&);
    void remove_window_button(::Window&, bool);
    void update_window_button(::Window&, bool);
    ::Window* find_window_owner(::Window&) const;

    virtual void event(Core::Event&) override;
    virtual void wm_event(GUI::WMEvent&) override;
    virtual void screen_rects_change_event(GUI::ScreenRectsChangeEvent&) override;

    void update_applet_area();

    NonnullRefPtr<GUI::Menu> m_start_menu;
    RefPtr<GUI::Widget> m_task_button_container;
    RefPtr<Gfx::Bitmap> m_default_icon;

    RefPtr<GUI::Button> m_start_button;
    RefPtr<GUI::Button> m_show_desktop_button;
};
