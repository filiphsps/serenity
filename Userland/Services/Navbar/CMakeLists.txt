serenity_component(
    Navbar
    REQUIRED
    TARGETS Navbar
)

set(SOURCES
    main.cpp
    NavbarButton.cpp
    NavbarWindow.cpp
    ../Taskbar/WindowList.cpp
)

serenity_bin(Navbar)
target_link_libraries(Navbar LibGUI LibDesktop LibMain)
serenity_install_headers(Services/Navbar)
