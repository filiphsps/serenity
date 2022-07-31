serenity_component(
    Statusbar
    REQUIRED
    TARGETS Statusbar
)

set(SOURCES
    main.cpp
    StatusbarWindow.cpp
    ../Taskbar/ClockWidget.cpp
)

serenity_bin(Statusbar)
target_link_libraries(Statusbar LibGUI LibDesktop LibConfig LibMain)
serenity_install_headers(Services/Statusbar)
