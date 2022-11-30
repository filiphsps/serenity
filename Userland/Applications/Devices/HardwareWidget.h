/*
 * Copyright (c) 2022, Filiph Sandström <filiph.sandstrom@filfatstudios.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibGUI/SettingsWindow.h>
#include <LibGUI/TreeView.h>
#include <LibGUI/Window.h>

class HardwareWidget final : public GUI::SettingsWindow::Tab {
    C_OBJECT(HardwareWidget)

public:
    virtual void apply_settings() override;
    virtual void reset_default_values() override;

private:
    HardwareWidget();

    RefPtr<GUI::TreeView> m_device_table_view;
};
