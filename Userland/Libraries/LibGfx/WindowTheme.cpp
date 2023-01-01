/*
 * Copyright (c) 2020, Andreas Kling <kling@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibGfx/Styles/Classic/ClassicWindowTheme.h>
#include <LibGfx/Styles/Modern/ModernWindowTheme.h>
#include <LibGfx/WindowTheme.h>

namespace Gfx {

WindowTheme& WindowTheme::current()
{
    static ModernWindowTheme theme;
    return theme;
}

}
