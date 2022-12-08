serenity_component(
    GlobalMenu
    REQUIRED
    TARGETS GlobalMenu
)

set(SOURCES
    main.cpp
    GlobalMenuWindow.cpp
)

serenity_bin(GlobalMenu)
target_link_libraries(GlobalMenu PRIVATE LibCore LibGfx LibGUI LibDesktop LibConfig LibIPC LibMain)
serenity_install_headers(Services/GlobalMenu)
