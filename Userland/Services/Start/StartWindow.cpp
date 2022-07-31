/*
 * Copyright (c) 2018-2021, Andreas Kling <kling@serenityos.org>
 * Copyright (c) 2021, Spencer Dixon <spencercdixon@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "StartWindow.h"
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

StartWindow::StartWindow()
{
    set_window_type(GUI::WindowType::Taskbar);
    set_title("Start");
    set_minimizable(true);

    on_screen_rects_change(GUI::Desktop::the().rects(), GUI::Desktop::the().main_screen_index());

    auto& main_widget = set_main_widget<StartWidget>();
    main_widget.set_layout<GUI::VerticalBoxLayout>();
    main_widget.layout()->set_margins({ 0, 0, 0, 0 });

    auto& scroll_container = main_widget.add<GUI::ScrollableContainerWidget>();
    auto container = GUI::Widget::construct();
    container->set_layout<GUI::VerticalBoxLayout>();
    container->layout()->set_margins({ 10, 5, 10, 5 });

    scroll_container.set_widget(container);

    Desktop::AppFile::for_each([&](auto af) {
        if (access(af->executable().characters(), X_OK) == 0) {
            apps.append({ af->executable(), af->name(), af->category(), af->icon(), af->run_in_terminal() });
        }
    });
    quick_sort(apps, [](auto& a, auto& b) { return a.name < b.name; });

    int rows = apps.size() / 3;
    dbgln("Apps: {}, Rows: {}", apps.size(), rows);

    for (int n = 1; n <= rows; n++) {
        auto& row = container->add<GUI::Widget>();
        row.set_layout<GUI::HorizontalBoxLayout>();
        row.layout()->set_margins({ 0, 5, 0, 5 });

        for (int i = 1; i <= 3; i++) {
            auto app = apps.at(i * n);

            auto& button = row.add<GUI::Button>(app.name);
            int size = (width() - 45) / 3;
            button.set_fixed_size(size, size);
            button.set_icon(app.icon.bitmap_for_size(112));
        }
    }
}

void StartWindow::config_string_did_change(String const&, String const&, String const&, String const&)
{
}

void StartWindow::on_screen_rects_change(Vector<Gfx::IntRect, 4> const& rects, size_t main_screen_index)
{
    auto const& rect = rects[main_screen_index];
    Gfx::IntRect new_rect { rect.x(), rect.top() + 36, rect.width() + 18, rect.height() - (36 + 74) + 18 };
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
