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

    void set_text(String label) {
        GUI::Button::set_text(label);

        if (label == "CatDog") {
            m_animation_enabled = true;
            m_tile_content = "Have you tried sleeping? It's good for you :^)";
        } else if (label == "Browser") {
            m_animation_enabled = true;
            m_tile_content_alignment = Gfx::TextAlignment::BottomLeft;
            m_tile_content = "SerenityOS News\nSerenityOS update (July 2022)";
        } else if (label == "Character Map") {
            m_animation_enabled = true;
            m_tile_content_alignment = Gfx::TextAlignment::BottomLeft;
            m_tile_content = "Character of the day: ඞ\nIt's kind of sus!";
        } else if (label == "Calculator") {
            m_animation_enabled = true;
            m_tile_content = "Upgrade to the programmer calculator pack for only $5.99";
        }
    }

private:
    explicit Tile();

    void tick();

    virtual void paint_event(GUI::PaintEvent&) override;
    virtual void timer_event(Core::TimerEvent&) override
    {
        tick();
    }

    String m_tile_content = "";
    Gfx::TextAlignment m_tile_content_alignment = Gfx::TextAlignment::Center;
    u32 m_animation_start = 0;
    bool m_animation_started = false;
    bool m_animation_enabled = false;
    u32 m_tick = 0;
    u32 m_round_trips = 0;
};
