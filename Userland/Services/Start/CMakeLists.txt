serenity_component(
    Start
    REQUIRED
    TARGETS Start
)

set(SOURCES
    GridLayout.cpp
    main.cpp
    StartWindow.cpp
    Tile.cpp
)

serenity_bin(Start)
target_link_libraries(Start LibGUI LibDesktop LibMain)
serenity_install_headers(Services/Start)
