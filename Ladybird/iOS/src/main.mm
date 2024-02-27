/*
 * Copyright (c) 2024, Filiph Sandström <filiph.sandstrom@filfatstudios.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Ladybird/Utilities.h>
#include <LibCore/EventLoop.h>
#include <LibGfx/Font/FontDatabase.h>
#include <LibMain/Main.h>
#include <syslog.h>

ErrorOr<int> serenity_main(Main::Arguments)
{
    dbgln("Hello World from Ladybird on iOS!");

    //Core::EventLoopManager::install(*new Ladybird::CFEventLoopManager);
    Core::EventLoop event_loop;

    platform_init();

    // NOTE: We only instantiate this to ensure that Gfx::FontDatabase has its default queries initialized.
    Gfx::FontDatabase::set_default_font_query("Katica 10 400 0");
    Gfx::FontDatabase::set_fixed_width_font_query("Csilla 10 400 0");

    dbgln("Entering the event loop :^)");
    return event_loop.exec();
}
