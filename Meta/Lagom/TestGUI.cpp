/*
 * Copyright (c) 2022, Filiph Sandström <filiph.sandstrom@filfatstudios.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibCore/System.h>
#include <LibGUI/Application.h>
#include <LibGUI/Window.h>
#include <LibGfx/Font/FontDatabase.h>
#include <LibGfx/Palette.h>
#include <LibGfx/SystemTheme.h>
#include <LibMain/Main.h>
#include <stdlib.h>

ErrorOr<int> serenity_main(Main::Arguments arguments)
{

    auto app = TRY(GUI::Application::try_create(arguments));

    auto window = TRY(GUI::Window::try_create());
    window->set_title("Hello LibGUI World");
    window->resize(600, 400);
    window->set_minimum_size(300, 245);

    window->show();
    return app->exec();
}
