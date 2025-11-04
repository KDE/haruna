#
# SPDX-FileCopyrightText: 2025 George Florea Bănuș <georgefb899@gmail.com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

qt_add_library(playlist STATIC)
qt_add_qml_module(playlist
    URI org.kde.haruna.playlist
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/src/org/kde/haruna/playlist
    IMPORT_PATH ${CMAKE_BINARY_DIR}
    QML_FILES
        Playlist.qml
        PlaylistItemDelegate.qml
        PlaylistItem.qml
        PlaylistItemCompact.qml
        PlaylistItemWithThumbnail.qml
        PlaylistTabDelegate.qml
    SOURCES
        playlistmodel.h
        playlistmodel.cpp
        playlistsortproxymodel.h
        playlistsortproxymodel.cpp
        playlistproxymodel.h
        playlistproxymodel.cpp
        playlistfilterproxymodel.h
        playlistfilterproxymodel.cpp
        playlistmultiproxiesmodel.h
        playlistmultiproxiesmodel.cpp
        playlistrenamevalidator.h
        playlistrenamevalidator.cpp
)

target_link_libraries(playlist PRIVATE
    Qt6::Core
    Qt6::Gui

    KF6::ConfigCore
    KF6::ConfigGui
    KF6::FileMetaData
    KF6::I18n
    KF6::KIOWidgets

    utilitiesplugin
    settingsplugin
    youtubeplugin
)
