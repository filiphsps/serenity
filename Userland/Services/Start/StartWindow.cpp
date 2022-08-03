/*
 * Copyright (c) 2022, Filiph Sandstrom <filiph.sandstrom@filfatstudios.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "GridLayout.h"
#include "StartWindow.h"
#include "Tile.h"
#include <AK/Debug.h>
#include <AK/QuickSort.h>
#include <LibGUI/BoxLayout.h>
#include <LibGUI/Button.h>
#include <LibGUI/ConnectionToWindowManagerServer.h>
#include <LibGUI/ConnectionToWindowServer.h>
#include <LibGUI/Desktop.h>
#include <LibGUI/Label.h>
#include <LibGUI/Icon.h>
#include <LibGUI/Menu.h>
#include <LibGUI/Painter.h>
#include <LibGUI/Window.h>
#include <LibGUI/ScrollableContainerWidget.h>
#include <LibGfx/Font/FontDatabase.h>
#include <LibGfx/Palette.h>
#include <serenity.h>
#include <stdio.h>

class StartWidget final : public GUI::Widget {
    C_OBJECT(StartWidget);

public:
    virtual ~StartWidget() override = default;

    virtual void paint_event(GUI::PaintEvent& event) override
    {
        GUI::Painter painter(*this);
        painter.add_clip_rect(event.rect());
        painter.fill_rect(rect(), palette().desktop_background());
    }

private:
    StartWidget() = default;
};

struct AppMetadata {
    String executable;
    String name;
    String category;
    GUI::Icon icon;
    bool run_in_terminal;
};
Vector<AppMetadata> apps;

const int ITEM_SIZE = 114;
StartWindow::StartWindow()
{
    set_window_type(GUI::WindowType::Taskbar);
    set_title("Start");
    set_minimizable(true);

    on_screen_rects_change(GUI::Desktop::the().rects(), GUI::Desktop::the().main_screen_index());

    auto& main_widget = set_main_widget<StartWidget>();
    main_widget.set_layout<GUI::VerticalBoxLayout>();
    main_widget.layout()->set_margins({ 0, 0, 0, 0 });

    auto container = GUI::Widget::construct();
    container->set_layout<GridLayout>();
    container->layout()->set_margins({ 4, 4, 4, 4 });
    static_cast<GridLayout*>(container->layout())->set_item_size(ITEM_SIZE);
    static_cast<GridLayout*>(container->layout())->set_columns(3);

    auto& scroll_container = main_widget.add<GUI::ScrollableContainerWidget>();
    scroll_container.set_should_hide_unnecessary_scrollbars(true);
    scroll_container.set_widget(container);

    Desktop::AppFile::for_each([&](auto af) {
        if (access(af->executable().characters(), X_OK) == 0 && af->category() != "Settings"sv && !af->run_in_terminal()) {
            apps.append({ af->executable(), af->name(), af->category(), af->icon(), af->run_in_terminal() });
        }
    });
    quick_sort(apps, [](auto& a, auto& b) { return a.name < b.name; });

    for (size_t n = 0; n < apps.size(); n++) {
        auto app = apps.at(n);

        auto& tile = container->add<Tile>();
        tile.set_text(app.name);
        tile.set_icon(app.icon.bitmap_for_size(ITEM_SIZE));
        tile.set_fixed_size(ITEM_SIZE, ITEM_SIZE);
    }
}

void StartWindow::config_string_did_change(String const&, String const&, String const&, String const&)
{
}

void StartWindow::on_screen_rects_change(Vector<Gfx::IntRect, 4> const& rects, size_t main_screen_index)
{
    auto const& rect = rects[main_screen_index];
    Gfx::IntRect new_rect { rect.x(), rect.top() + 33, rect.width() + 18, rect.height() - (36 + 74) + 3 };
    set_rect(new_rect);
}

void StartWindow::event(Core::Event& event)
{
    Window::event(event);
}

void StartWindow::wm_event(GUI::WMEvent& event)
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
    default:
        break;
    }
}

void StartWindow::screen_rects_change_event(GUI::ScreenRectsChangeEvent& event)
{
    on_screen_rects_change(event.rects(), event.main_screen_index());
}
