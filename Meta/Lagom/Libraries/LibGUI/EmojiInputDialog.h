/*
 * Copyright (c) 2022, Filiph Sandström <filiph.sandstrom@filfatstudios.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibGUI/Dialog.h>

namespace GUI {

class EmojiInputDialog final : public Dialog {
    C_OBJECT(EmojiInputDialog);

public:
    String const& selected_emoji_text() const { return m_selected_emoji_text; }

private:
    explicit EmojiInputDialog(Window* parent_window);
    String m_selected_emoji_text;
};

}
