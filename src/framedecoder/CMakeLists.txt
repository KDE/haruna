#
# SPDX-FileCopyrightText: 2025 George Florea Bănuș <georgefb899@gmail.com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

qt_add_library(framedecoder STATIC)
qt_add_qml_module(framedecoder
    URI org.kde.haruna.framedecoder
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/src/org/kde/haruna/framedecoder
    IMPORT_PATH ${CMAKE_BINARY_DIR}
    SOURCES
        framedecoder.h
        framedecoder.cpp
)

ecm_qt_declare_logging_category(framedecoder
    HEADER "logging/_framedecoder.h"
    IDENTIFIER "HarunaFrameDecoder"
    CATEGORY_NAME "org.kde.haruna.framedecoder"
    DEFAULT_SEVERITY Info
)

target_include_directories(framedecoder
    PRIVATE
        ${FFMPEG_INCLUDE_DIRS}
)

target_link_libraries(framedecoder PRIVATE
    Qt6::Core
    Qt6::Gui

    ${AVUTIL_LIBRARIES}
    ${AVFILTER_LIBRARIES}
    ${AVFORMAT_LIBRARIES}
    ${AVCODEC_LIBRARIES}
    ${SWSCALE_LIBRARIES}
)
