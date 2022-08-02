/*
 * Copyright (c) 2022, Filiph Sandstrom <filiph.sandstrom@filfatstudios.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "StartWindow.h"
#include <AK/Debug.h>
#include <AK/QuickSort.h>
#include <LibConfig/Client.h>
#include <LibCore/ConfigFile.h>
#include <LibCore/EventLoop.h>
#include <LibCore/StandardPaths.h>
#include <LibCore/System.h>
#include <LibDesktop/AppFile.h>
#include <LibDesktop/Launcher.h>
#include <LibGUI/ActionGroup.h>
#include <LibGUI/Application.h>
#include <LibGUI/ConnectionToWindowManagerServer.h>
#include <LibGUI/ConnectionToWindowServer.h>
#include <LibGUI/Menu.h>
#include <LibGUI/MenuItem.h>
#include <LibGUI/Process.h>
#include <LibGfx/SystemTheme.h>
#include <LibMain/Main.h>
#include <WindowServer/Window.h>
#include <serenity.h>
#include <signal.h>
#include <spawn.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

ErrorOr<int> serenity_main(Main::Arguments arguments)
{
    TRY(Core::System::pledge("stdio recvfd sendfd proc exec rpath unix"));
    auto app = TRY(GUI::Application::try_create(arguments));
    Config::pledge_domain("Start");
    Config::monitor_domain("Start");

    GUI::ConnectionToWindowManagerServer::the();
    Desktop::Launcher::ensure_connection();

    TRY(Core::System::pledge("stdio recvfd sendfd proc exec rpath"));

    auto window = TRY(StartWindow::try_create());

    window->show();
    window->make_window_manager(
        WindowServer::WMEventMask::WindowStateChanges
        | WindowServer::WMEventMask::WindowRemovals
        | WindowServer::WMEventMask::WindowIconChanges
        | WindowServer::WMEventMask::WorkspaceChanges);

    return app->exec();
}
