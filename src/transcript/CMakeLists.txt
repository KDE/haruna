#
# SPDX-FileCopyrightText: 2026 George Florea Bănuș <georgefb899@gmail.com>
# SPDX-FileCopyrightText: 2026 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

qt_add_library(transcript STATIC)
qt_add_qml_module(transcript
    URI org.kde.haruna.transcript
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/src/org/kde/haruna/transcript
    IMPORT_PATH ${CMAKE_BINARY_DIR}
    QML_FILES
        Transcript.qml
)

target_link_libraries(transcript PRIVATE
    Qt6::Core
)
