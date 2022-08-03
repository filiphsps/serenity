/*
 * Copyright (c) 2022, Filiph Sandstrom <filiph.sandstrom@filfatstudios.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/String.h>
#include <AK/Vector.h>
#include <LibGUI/Button.h>
#include <LibGfx/Bitmap.h>

class Tile final : public GUI::Button {
    C_OBJECT(Tile);

public:
    virtual ~Tile() override = default;

    static int animation_idle() { return 60 * 5; }
    static int animation_duration() { return 60 * 1; }

    struct TileContent {
        enum class ContentKind {
            Branding,
            Normal,
            Date
        };
        ContentKind content_kind;

        enum class ContentAlignment {
            Center,
            Bottom
        };
        ContentAlignment content_alignment = ContentAlignment::Bottom;

        String content = String::empty();
    };
    Vector<TileContent> contents() { return m_contents; }
    void set_contents(Vector<TileContent> contents) { m_contents = contents; }

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

            m_contents.append({
                TileContent::ContentKind::Normal,
                TileContent::ContentAlignment::Center,
                String("You should get a cowboy hat")
            });
            m_contents.append({
                TileContent::ContentKind::Normal,
                TileContent::ContentAlignment::Center,
                String("Have you tried sleeping? It's good for you :^)")
            });
        } else if (label == "Browser") {
            m_animation = TileAnimation::Slide;

            m_contents.append({
                TileContent::ContentKind::Normal,
                TileContent::ContentAlignment::Bottom,
                String("SerenityOS News\nSerenityOS update (July 2022)")
            });
        } else if (label == "Calculator") {
            m_animation = TileAnimation::Slide;

            m_contents.append({
                TileContent::ContentKind::Normal,
                TileContent::ContentAlignment::Bottom,
                String("Upgrade to the programmer calculator pack for only $5.99")
            });
        } else if (label == "Calendar") {
            m_branding = TileBranding::None;

            m_contents.append({
                TileContent::ContentKind::Date,
            });
        } else if (label == "Character Map") {
            m_animation = TileAnimation::Slide;

            m_contents.append({
                TileContent::ContentKind::Normal,
                TileContent::ContentAlignment::Bottom,
                String("Character of the day: ඞ\nIt's kind of sus!")
            });
        }
    }

private:
    explicit Tile();

    void tick();

    struct Animation {
        Gfx::IntRect previous_rect;
        Gfx::IntRect current_rect;
    };

    virtual void paint_event(GUI::PaintEvent&) override;
    virtual void timer_event(Core::TimerEvent&) override
    {
        tick();
    }
    
    void draw_branding_tile(GUI::Painter painter, Gfx::IntRect content_rect);
    void draw_normal_tile(GUI::Painter painter, Gfx::IntRect content_rect, TileContent content);
    void draw_date_tile(GUI::Painter painter, Gfx::IntRect content_rect);

    void tick_tile(GUI::Painter painter);
    Animation process_animation(int tick);

    TileAnimation m_animation = TileAnimation::None;
    TileBranding m_branding = TileBranding::Label;

    Vector<TileContent> m_contents;

    int m_animation_start = 0;
    bool m_animation_started = false;
    int m_tick = 0;
    int m_round_trips = 0;
};
