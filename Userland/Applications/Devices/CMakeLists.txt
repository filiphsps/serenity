serenity_component(
    Devices
    RECOMMENDED
    TARGETS Devices
)

compile_gml(HardwareWidget.gml HardwareWidgetGML.h hardware_widget_gml)

set(SOURCES
    main.cpp
    HardwareModel.cpp
    HardwareWidget.cpp
)

set(GENERATED_SOURCES
    HardwareWidgetGML.h
)

serenity_app(Devices ICON app-profiler)
target_link_libraries(Devices PRIVATE LibCore LibPCIDB LibGfx LibGUI LibMain)
