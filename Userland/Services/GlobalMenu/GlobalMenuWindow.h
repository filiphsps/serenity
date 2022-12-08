/*
 * Copyright (c) 2018-2020, Andreas Kling <kling@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibConfig/Listener.h>
#include <LibGUI/Widget.h>
#include <LibGUI/Window.h>
#include <LibGfx/ShareableBitmap.h>
#include <Services/WindowServer/ScreenLayout.h>

class GlobalMenuWindow final : public GUI::Window
    , public Config::Listener {
    C_OBJECT(GlobalMenuWindow);

public:
    virtual ~GlobalMenuWindow() override = default;

    static int menu_height() { return 22; }

    virtual void config_string_did_change(DeprecatedString const&, DeprecatedString const&, DeprecatedString const&, DeprecatedString const&) override;

private:
    explicit GlobalMenuWindow();

    void on_screen_rects_change(Vector<Gfx::IntRect, 4> const&, size_t);

    virtual void event(Core::Event&) override;
    virtual void wm_event(GUI::WMEvent&) override;
    virtual void screen_rects_change_event(GUI::ScreenRectsChangeEvent&) override;

    void update_menu_area();

    Gfx::IntSize m_menu_area_size;
    RefPtr<GUI::Frame> m_menu_area_container;
};
