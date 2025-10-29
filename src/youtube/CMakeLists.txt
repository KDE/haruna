
qt_add_library(youtube STATIC)
qt_add_qml_module(youtube
    URI org.kde.haruna.youtube
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/src/org/kde/haruna/youtube
    IMPORT_PATH ${CMAKE_BINARY_DIR}
    SOURCES
        youtube.h
        youtube.cpp
)

target_link_libraries(youtube PRIVATE
    KF6::I18n
)
