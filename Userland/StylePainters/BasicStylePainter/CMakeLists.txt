set(SOURCES
    BasicStylePainter.cpp
)

serenity_stylepainter(BasicStylePainter)
target_link_libraries(BasicStylePainter LibC LibGfx)
