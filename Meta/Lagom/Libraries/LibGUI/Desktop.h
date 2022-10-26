/*
 * Copyright (c) 2022, Filiph Sandström <filiph.sandstrom@filfatstudios.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Function.h>
#include <AK/String.h>
#include <LibGUI/Forward.h>
#include <LibGUI/SystemEffects.h>
#include <LibGfx/Rect.h>

namespace GUI {

class Desktop {
public:
    static Desktop& the();
    Desktop() = default;

    Gfx::IntRect rect() const
    {
        return Gfx::IntRect(0, 0, 0, 0);
    }

    int taskbar_height() const { return 0; }

    void set_system_effects(Vector<bool> effects) { m_system_effects = { effects }; };
    SystemEffects const& system_effects() const { return m_system_effects; }

private:
    SystemEffects m_system_effects;
};

}
