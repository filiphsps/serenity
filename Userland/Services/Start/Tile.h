/*
 * Copyright (c) 2022, Filiph Sandstrom <filiph.sandstrom@filfatstudios.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/String.h>
#include <LibGUI/Button.h>
#include <LibGfx/Bitmap.h>

class Tile final : public GUI::Button {
    C_OBJECT(Tile);

public:
    virtual ~Tile() override = default;

    static u32 animation_idle() { return 60 * 5; }
    static u32 animation_duration() { return 60 * 2; }

private:
    explicit Tile();

    void tick();

    virtual void paint_event(GUI::PaintEvent&) override;
    virtual void timer_event(Core::TimerEvent&) override
    {
        tick();
    }

    u32 m_animation_start = 0;
    bool m_animation_started = false;
    bool m_animation_enabled = true;
    u32 m_tick = 0;
};
