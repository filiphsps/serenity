/*
 * Copyright (c) 2022, Filiph Sandstrom <filiph.sandstrom@filfatstudios.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibConfig/Listener.h>
#include <LibGUI/Widget.h>
#include <LibGUI/Window.h>
#include <LibGfx/ShareableBitmap.h>
#include <Services/WindowServer/ScreenLayout.h>

class StartWindow final : public GUI::Window
    , public Config::Listener {
    C_OBJECT(StartWindow);

public:
    virtual ~StartWindow() override = default;

    virtual void config_string_did_change(String const&, String const&, String const&, String const&) override;

private:
    explicit StartWindow();

    virtual void event(Core::Event&) override;
    virtual void wm_event(GUI::WMEvent&) override;
    virtual void screen_rects_change_event(GUI::ScreenRectsChangeEvent&) override;

    void on_screen_rects_change(Vector<Gfx::IntRect, 4> const&, size_t);
};
