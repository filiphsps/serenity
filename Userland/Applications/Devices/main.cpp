/*
 * Copyright (c) 2022, Filiph Sandström <filiph.sandstrom@filfatstudios.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "HardwareWidget.h"
#include <LibCore/System.h>
#include <LibGUI/Application.h>
#include <LibGUI/Icon.h>
#include <LibGUI/SettingsWindow.h>
#include <LibMain/Main.h>

ErrorOr<int> serenity_main(Main::Arguments arguments)
{
    TRY(Core::System::pledge("stdio rpath recvfd sendfd unix"));

    auto app = TRY(GUI::Application::try_create(arguments));

    TRY(Core::System::pledge("stdio rpath recvfd sendfd"));
    // TRY(Core::System::unveil("/res", "r"));
    // TRY(Core::System::unveil("/sys/bus/pci/", "r"));
    // TRY(Core::System::unveil(nullptr, nullptr));

    auto app_icon = GUI::Icon::default_icon("app-profiler"sv);

    auto window = TRY(GUI::SettingsWindow::create("Devices"));
    (void)TRY(window->add_tab<HardwareWidget>("Hardware"sv, "hardware"sv));
    window->set_icon(app_icon.bitmap_for_size(16));

    window->show();
    return app->exec();
}
