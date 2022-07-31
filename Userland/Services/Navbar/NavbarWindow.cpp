/*
 * Copyright (c) 2018-2021, Andreas Kling <kling@serenityos.org>
 * Copyright (c) 2021, Spencer Dixon <spencercdixon@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "NavbarWindow.h"
#include "NavbarButton.h"
#include <AK/Debug.h>
#include <LibGUI/BoxLayout.h>
#include <LibGUI/Button.h>
#include <LibGUI/ConnectionToWindowManagerServer.h>
#include <LibGUI/ConnectionToWindowServer.h>
#include <LibGUI/Desktop.h>
#include <LibGUI/Frame.h>
#include <LibGUI/Icon.h>
#include <LibGUI/Menu.h>
#include <LibGUI/Painter.h>
#include <LibGUI/Window.h>
#include <LibGfx/Font/FontDatabase.h>
#include <LibGfx/Palette.h>
#include <serenity.h>
#include <stdio.h>

class NavbarWidget final : public GUI::Widget {
    C_OBJECT(NavbarWidget);

public:
    virtual ~NavbarWidget() override = default;

private:
    NavbarWidget() = default;

    virtual void paint_event(GUI::PaintEvent& event) override
    {
        GUI::Painter painter(*this);
        painter.add_clip_rect(event.rect());
        painter.fill_rect(rect(), palette().button());
        painter.draw_line({ 0, 1 }, { width() - 1, 1 }, palette().threed_highlight());
    }

    virtual void did_layout() override
    {
        WindowList::the().for_each_window([&](auto& window) {
            if (auto* button = window.button())
                static_cast<NavbarButton*>(button)->update_taskbar_rect();
        });
    }
};

NavbarWindow::NavbarWindow(NonnullRefPtr<GUI::Menu> start_menu)
    : m_start_menu(move(start_menu))
{
    set_window_type(GUI::WindowType::Taskbar);
    set_title("Navbar");

    on_screen_rects_change(GUI::Desktop::the().rects(), GUI::Desktop::the().main_screen_index());

    auto& main_widget = set_main_widget<NavbarWidget>();
    main_widget.set_layout<GUI::HorizontalBoxLayout>();
    main_widget.layout()->set_margins({ 5, 0, 5, 0 });

    auto container = GUI::Widget::construct();
    container->set_layout<GUI::HorizontalBoxLayout>();
    container->layout()->set_margins({ 5, 15, 5, 15 });
    main_widget.add_child(*container);

    auto app_icon = GUI::Icon::default_icon("ladyball"sv);

    auto padding = GUI::Widget::construct();
    padding->set_fixed_size(width() / 2 - 108, 58);
    container->add_child(*padding);

    auto m_back_button = GUI::Button::construct("Back");
    m_back_button->set_fixed_size(58, 58);
    container->add_child(*m_back_button);

    m_start_button = GUI::Button::construct();
    m_start_button->set_fixed_size(58, 58);
    m_start_button->set_icon(app_icon.bitmap_for_size(42));
    m_start_button->set_menu(m_start_menu);
    container->add_child(*m_start_button);

    auto m_task_button = GUI::Button::construct("Search");
    m_task_button->set_fixed_size(58, 58);
    container->add_child(*m_task_button);

    container->add_child(*padding);

    /*m_task_button_container = main_widget.add<GUI::Widget>();
    m_task_button_container->set_layout<GUI::HorizontalBoxLayout>();
    m_task_button_container->layout()->set_spacing(3);

    m_default_icon = Gfx::Bitmap::try_load_from_file("/res/icons/16x16/window.png"sv).release_value_but_fixme_should_propagate_errors();

    m_show_desktop_button = GUI::Button::construct();
    m_show_desktop_button->set_tooltip("Show Desktop");
    m_show_desktop_button->set_icon(GUI::Icon::default_icon("desktop"sv).bitmap_for_size(42));
    m_show_desktop_button->set_button_style(Gfx::ButtonStyle::Coolbar);
    m_show_desktop_button->set_fixed_size(58, 58);
    m_show_desktop_button->on_click = NavbarWindow::show_desktop_button_clicked;
    main_widget.add_child(*m_show_desktop_button);*/
}

void NavbarWindow::show_desktop_button_clicked(unsigned)
{
    toggle_show_desktop();
}

void NavbarWindow::toggle_show_desktop()
{
    /*WindowList::the().for_each_window([&](auto& window) {
        if (window.title() != "Start")
            return;
        
        window.set_minimized(!window.is_minimized());
    });*/
}

void NavbarWindow::on_screen_rects_change(Vector<Gfx::IntRect, 4> const& rects, size_t main_screen_index)
{
    auto const& rect = rects[main_screen_index];
    Gfx::IntRect new_rect { rect.x(), rect.bottom() - navbar_height() + 1, rect.width(), navbar_height() };
    set_rect(new_rect);
}

NonnullRefPtr<GUI::Button> NavbarWindow::create_button(WindowIdentifier const& identifier)
{
    auto& button = m_task_button_container->add<NavbarButton>(identifier);
    button.set_min_size(58, 58);
    button.set_max_size(58, 58);
    button.set_text_alignment(Gfx::TextAlignment::BottomCenter);
    button.set_text("");
    button.set_icon(*m_default_icon);
    return button;
}

void NavbarWindow::add_window_button(::Window&, WindowIdentifier const&)
{
    /*if (window.button())
        return;
    window.set_button(create_button(identifier));
    auto* button = window.button();
    button->on_click = [window = &window, identifier, button](auto) {
        // We need to look at the button's checked state here to figure
        // out if the application is active or not. That's because this
        // button's window may not actually be active when a modal window
        // is displayed, in which case window->is_active() would return
        // false because window is the modal window's owner (which is not
        // active)
        if (window->is_minimized() || !button->is_checked()) {
            GUI::ConnectionToWindowManagerServer::the().async_set_active_window(identifier.client_id(), identifier.window_id());
        } else {
            GUI::ConnectionToWindowManagerServer::the().async_set_window_minimized(identifier.client_id(), identifier.window_id(), true);
        }
    };*/
}

void NavbarWindow::remove_window_button(::Window& window, bool was_removed)
{
    auto* button = window.button();
    if (!button)
        return;
    if (!was_removed)
        static_cast<NavbarButton*>(button)->clear_taskbar_rect();
    window.set_button(nullptr);
    button->remove_from_parent();
}

void NavbarWindow::update_window_button(::Window& window, bool show_as_active)
{
    auto* button = window.button();
    if (!button)
        return;
    button->set_text(window.title());
    button->set_tooltip(window.title());
    button->set_checked(show_as_active);
}

::Window* NavbarWindow::find_window_owner(::Window& window) const
{
    if (!window.is_modal())
        return &window;

    ::Window* parent = nullptr;
    auto* current_window = &window;
    while (current_window) {
        parent = WindowList::the().find_parent(*current_window);
        if (!parent || !parent->is_modal())
            break;
        current_window = parent;
    }
    return parent;
}

void NavbarWindow::event(Core::Event& event)
{
    switch (event.type()) {
    case GUI::Event::MouseDown: {
        // If the cursor is at the edge/corner of the screen but technically not within the start button (or other taskbar buttons),
        // we adjust it so that the nearest button ends up being clicked anyways.

        auto& mouse_event = static_cast<GUI::MouseEvent&>(event);
        int const ADJUSTMENT = 4;
        auto adjusted_x = AK::clamp(mouse_event.x(), ADJUSTMENT, width() - ADJUSTMENT);
        auto adjusted_y = AK::min(mouse_event.y(), height() - ADJUSTMENT);
        Gfx::IntPoint adjusted_point = { adjusted_x, adjusted_y };

        if (adjusted_point != mouse_event.position()) {
            GUI::ConnectionToWindowServer::the().async_set_global_cursor_position(position() + adjusted_point);
            GUI::MouseEvent adjusted_event = { (GUI::Event::Type)mouse_event.type(), adjusted_point, mouse_event.buttons(), mouse_event.button(), mouse_event.modifiers(), mouse_event.wheel_delta_x(), mouse_event.wheel_delta_y(), mouse_event.wheel_raw_delta_x(), mouse_event.wheel_raw_delta_y() };
            Window::event(adjusted_event);
            return;
        }
        break;
    }
    }
    Window::event(event);
}

void NavbarWindow::wm_event(GUI::WMEvent& event)
{
    WindowIdentifier identifier { event.client_id(), event.window_id() };
    switch (event.type()) {
    case GUI::Event::WM_WindowRemoved: {
        if constexpr (EVENT_DEBUG) {
            auto& removed_event = static_cast<GUI::WMWindowRemovedEvent&>(event);
            dbgln("WM_WindowRemoved: client_id={}, window_id={}",
                removed_event.client_id(),
                removed_event.window_id());
        }
        if (auto* window = WindowList::the().window(identifier))
            remove_window_button(*window, true);
        WindowList::the().remove_window(identifier);
        update();
        break;
    }
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

    case GUI::Event::WM_WindowIconBitmapChanged: {
        auto& changed_event = static_cast<GUI::WMWindowIconBitmapChangedEvent&>(event);
        if (auto* window = WindowList::the().window(identifier)) {
            if (window->button()) {
                auto icon = changed_event.bitmap();
                if (icon->height() != navbar_icon_size() || icon->width() != navbar_icon_size()) {
                    auto sw = navbar_icon_size() / (float)icon->width();
                    auto sh = navbar_icon_size() / (float)icon->height();
                    auto scaled_bitmap_or_error = icon->scaled(sw, sh);
                    if (scaled_bitmap_or_error.is_error())
                        window->button()->set_icon(nullptr);
                    else
                        window->button()->set_icon(scaled_bitmap_or_error.release_value());
                } else {
                    window->button()->set_icon(icon);
                }
            }
        }
        break;
    }

    case GUI::Event::WM_WindowStateChanged: {
        auto& changed_event = static_cast<GUI::WMWindowStateChangedEvent&>(event);
        if constexpr (EVENT_DEBUG) {
            dbgln("WM_WindowStateChanged: client_id={}, window_id={}, title={}, rect={}, is_active={}, is_minimized={}",
                changed_event.client_id(),
                changed_event.window_id(),
                changed_event.title(),
                changed_event.rect(),
                changed_event.is_active(),
                changed_event.is_minimized());
        }
        if (changed_event.window_type() != GUI::WindowType::Normal || changed_event.is_frameless()) {
            if (auto* window = WindowList::the().window(identifier))
                remove_window_button(*window, false);
            break;
        }
        auto& window = WindowList::the().ensure_window(identifier);
        window.set_parent_identifier({ changed_event.parent_client_id(), changed_event.parent_window_id() });
        if (!window.is_modal())
            add_window_button(window, identifier);
        else
            remove_window_button(window, false);
        window.set_title(changed_event.title());
        window.set_rect(changed_event.rect());
        window.set_modal(changed_event.is_modal());
        window.set_active(changed_event.is_active());
        window.set_minimized(changed_event.is_minimized());
        window.set_progress(changed_event.progress());

        auto* window_owner = find_window_owner(window);
        if (window_owner == &window) {
            update_window_button(window, window.is_active());
        } else if (window_owner) {
            // check the window owner's button if the modal's window button
            // would have been checked
            VERIFY(window.is_modal());
            update_window_button(*window_owner, window.is_active());
        }
        break;
    }

    default:
        break;
    }
}

void NavbarWindow::screen_rects_change_event(GUI::ScreenRectsChangeEvent& event)
{
    on_screen_rects_change(event.rects(), event.main_screen_index());
}
