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

    static int animation_idle() { return 60 * 5; }
    static int animation_duration() { return 60 * 1; }

    enum class TileKind {
        Normal,
        Date
    };

    enum class TileAnimation {
        None,
        Slide
    };

    bool animated() { return m_animation != TileAnimation::None; }
    TileAnimation animation() { return m_animation; }
    void set_animation(TileAnimation animation) { m_animation = animation; }

    enum class TileBranding {
        None,
        Label
    };

    TileBranding branding() { return m_branding; }
    void set_branding(TileBranding branding) { m_branding = branding; }

    void set_text(String label) {
        GUI::Button::set_text(label);

        if (label == "CatDog") {
            m_animation = TileAnimation::Slide;
            m_content = "Have you tried sleeping? It's good for you :^)";
        } else if (label == "Browser") {
            m_animation = TileAnimation::Slide;
            m_content_alignment = Gfx::TextAlignment::BottomLeft;
            m_content = "SerenityOS News\nSerenityOS update (July 2022)";
        } else if (label == "Calculator") {
            m_animation = TileAnimation::Slide;
            m_content = "Upgrade to the programmer calculator pack for only $5.99";
        } else if (label == "Calendar") {
            m_kind = TileKind::Date;
            m_branding = TileBranding::None;
        } else if (label == "Character Map") {
            m_animation = TileAnimation::Slide;
            m_content_alignment = Gfx::TextAlignment::BottomLeft;
            m_content = "Character of the day: ඞ\nIt's kind of sus!";
        }
    }

private:
    explicit Tile();

    void tick();

    struct Animation {
        Gfx::IntRect icon_rect;
        Gfx::IntRect content_rect;
        bool draw_content = false;
    };

    virtual void paint_event(GUI::PaintEvent&) override;
    virtual void timer_event(Core::TimerEvent&) override
    {
        tick();
    }
    
    void draw_normal_tile(GUI::Painter painter, Gfx::IntRect icon_rect, Gfx::IntRect content_rect, bool draw_content = false);
    void draw_date_tile(GUI::Painter painter);
    void draw_tile(GUI::Painter painter, Tile::Animation animation_state);

    Animation process_animation();

    TileKind m_kind = TileKind::Normal;
    TileAnimation m_animation = TileAnimation::None;
    TileBranding m_branding = TileBranding::Label;

    String m_content = "";
    Gfx::TextAlignment m_content_alignment = Gfx::TextAlignment::Center;
    int m_animation_start = 0;
    bool m_animation_started = false;
    int m_tick = 0;
    int m_round_trips = 0;
};
