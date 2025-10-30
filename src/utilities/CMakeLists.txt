
find_package(Qt6 REQUIRED COMPONENTS Core)

qt_add_library(utilities STATIC)
qt_add_qml_module(utilities
    URI org.kde.haruna.utilities
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/src/org/kde/haruna/utilities
    IMPORT_PATH ${CMAKE_BINARY_DIR}
    SOURCES
        pathutils.h
        pathutils.cpp
        systemutils.h
        systemutils.cpp
)

target_link_libraries(utilities PRIVATE
    Qt6::Core

    KF6::ConfigCore
    KF6::KIOWidgets
    KF6::WindowSystem
)
