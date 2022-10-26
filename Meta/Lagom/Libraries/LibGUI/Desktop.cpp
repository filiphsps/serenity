/*
 * Copyright (c) 2022, Filiph Sandström <filiph.sandstrom@filfatstudios.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibGUI/Desktop.h>

namespace GUI {

Desktop& Desktop::the()
{
    static Desktop s_the;
    return s_the;
}

}
