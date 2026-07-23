#
# SPDX-FileCopyrightText: 2026 George Florea Bănuș <georgefb899@gmail.com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

qt_add_library(mpris STATIC)
target_sources(mpris
    PRIVATE
        mpriscontroller.h mpriscontroller.cpp
        playeradaptor.h playeradaptor.cpp
        rootadaptor.h rootadaptor.cpp
)

target_link_libraries(mpris PRIVATE
    Qt6::DBus
)
