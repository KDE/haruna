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
        PlaylistAdvancedSortWindow.qml
        PlaylistItem.qml
        PlaylistItemContextMenu.qml
        PlaylistItemCompact.qml
        PlaylistItemDelegate.qml
        PlaylistItemWithThumbnail.qml
        PlaylistSectionDelegate.qml
        PlaylistTabBar.qml
        PlaylistTabDelegate.qml
    SOURCES
        m3uparser.h
        m3uparser.cpp
        playlistmodel.h
        playlistmodel.cpp
        playlistsmanager.h
        playlistsmanager.cpp
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
        playlistmetadata.h
        playlistsortpropertymodel.h
        playlistsortpropertymodel.cpp
        playlistsortpropertyproxymodel.h
        playlistsortpropertyproxymodel.cpp
)

ecm_qt_declare_logging_category(playlist
    HEADER "logging/_playlist.h"
    IDENTIFIER "HarunaPlaylist"
    CATEGORY_NAME "org.kde.haruna.playlist"
    DEFAULT_SEVERITY Info
)

target_link_libraries(playlist PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Sql

    KF6::ConfigCore
    KF6::ConfigGui
    KF6::FileMetaData
    KF6::I18n
    KF6::KIOWidgets

    utilitiesplugin
    settingsplugin
    youtubeplugin
)
