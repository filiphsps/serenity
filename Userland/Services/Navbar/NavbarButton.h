/*
 * Copyright (c) 2018-2020, Andreas Kling <kling@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "../Taskbar/WindowIdentifier.h"
#include <LibGUI/Button.h>

class NavbarButton final : public GUI::Button {
    C_OBJECT(NavbarButton)
public:
    virtual ~NavbarButton() override = default;

    void update_taskbar_rect();
    void clear_taskbar_rect();

private:
    explicit NavbarButton(WindowIdentifier const&);

    virtual void context_menu_event(GUI::ContextMenuEvent&) override;
    virtual void resize_event(GUI::ResizeEvent&) override;
    virtual void paint_event(GUI::PaintEvent&) override;

    WindowIdentifier m_identifier;
};
