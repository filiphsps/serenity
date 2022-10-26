/*
 * Copyright (c) 2018-2021, Andreas Kling <kling@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Forward.h>

namespace GUI {

class ConnectionToWindowManagerServer {
public:
    static ConnectionToWindowManagerServer& the()
    {
        static ConnectionToWindowManagerServer s_the;
        return s_the;
    }

    void async_set_event_mask(i32) { }
    void async_set_manager_window(i32) { }

private:
    ConnectionToWindowManagerServer() = default;

};

}
