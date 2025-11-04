#
# SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

# DATA_ICONS is defined in data/CMakeLists.txt
ecm_add_app_icon(ICONS_SRCS ICONS ${DATA_ICONS})

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(haruna)

target_sources(haruna
    PRIVATE
    application.h application.cpp
    database.h database.cpp
    lockmanager.h lockmanager.cpp
    main.cpp
    models/actionsmodel.h models/actionsmodel.cpp
    models/chaptersmodel.h models/chaptersmodel.cpp
    models/customcommandsmodel.h models/customcommandsmodel.cpp
    models/mouseactionsmodel.h models/mouseactionsmodel.cpp
    models/recentfilesmodel.h models/recentfilesmodel.cpp
    models/subtitlesfoldersmodel.h models/subtitlesfoldersmodel.cpp
    models/tracksmodel.h models/tracksmodel.cpp
    models/playlist/playlistmodel.h models/playlist/playlistmodel.cpp
    models/playlist/playlistsortproxymodel.h models/playlist/playlistsortproxymodel.cpp
    models/playlist/playlistproxymodel.h models/playlist/playlistproxymodel.cpp
    models/playlist/playlistfilterproxymodel.h models/playlist/playlistfilterproxymodel.cpp
    models/playlist/playlistmultiproxiesmodel.h models/playlist/playlistmultiproxiesmodel.cpp
    models/playlist/playlistrenamevalidator.h models/playlist/playlistrenamevalidator.cpp
    mpv/mpvitem.h mpv/mpvitem.cpp
    mpv/mpvpreview.h mpv/mpvpreview.cpp
    mpv/mpvproperties.h
    thumbnailimageprovider.h thumbnailimageprovider.cpp
    worker.h worker.cpp
    ${ICONS_SRCS}
)

qt_add_resources(haruna sql_files
    FILES
        sql/create-recent_files-table.sql
        sql/create-playback_position-table.sql
)

if (CMAKE_SYSTEM_NAME IN_LIST DBUS_PLATFORMS)
    qt_add_dbus_interface(haruna_screensaver_inhibit_SRCS ../org.freedesktop.ScreenSaver.xml screensaverdbusinterface)
    target_sources(haruna
        PRIVATE
        mpris2/mediaplayer2.h mpris2/mediaplayer2.cpp
        mpris2/mediaplayer2player.h mpris2/mediaplayer2player.cpp
        ${haruna_screensaver_inhibit_SRCS}
    )
    target_include_directories(haruna
        PRIVATE
            mpris2
            ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()

add_subdirectory(framedecoder)
add_subdirectory(utilities)
add_subdirectory(settings)
add_subdirectory(youtube)

qt_policy(SET QTP0001 NEW)

qt_add_qml_module(haruna
    URI org.kde.haruna
    VERSION 1.0
    QML_FILES
        qml/Actions.qml
        qml/Footer.qml
        qml/Header.qml
        qml/HProgressBar.qml
        qml/MenuBarLoader.qml
        qml/MpvVideo.qml
        qml/Osd.qml
        qml/PlayList.qml
        qml/VolumeSlider.qml
        qml/Main.qml

        qml/Haruna/Components/AudioTracksMenu.qml
        qml/Haruna/Components/ColorPickerButton.qml
        qml/Haruna/Components/HamburgerMenu.qml
        qml/Haruna/Components/ImageAdjustmentSlider.qml
        qml/Haruna/Components/InputPopup.qml
        qml/Haruna/Components/LabelWithTooltip.qml
        qml/Haruna/Components/SelectActionPopup.qml
        qml/Haruna/Components/SettingsBasePage.qml
        qml/Haruna/Components/SettingsHeader.qml
        qml/Haruna/Components/SubtitlesFolders.qml
        qml/Haruna/Components/SubtitleTracksMenu.qml
        qml/Haruna/Components/ToolTipButton.qml

        qml/Haruna/PlaylistItems/PlaylistItemDelegate.qml
        qml/Haruna/PlaylistItems/PlayListItem.qml
        qml/Haruna/PlaylistItems/PlayListItemCompact.qml
        qml/Haruna/PlaylistItems/PlayListItemWithThumbnail.qml
        qml/Haruna/PlaylistItems/PlaylistTabDelegate.qml

        qml/Menus/AudioMenu.qml
        qml/Menus/MpvContextMenu.qml
        qml/Menus/FileMenu.qml
        qml/Menus/HelpMenu.qml
        qml/Menus/PlaybackMenu.qml
        qml/Menus/SettingsMenu.qml
        qml/Menus/SubtitlesMenu.qml
        qml/Menus/VideoMenu.qml
        qml/Menus/ViewMenu.qml
        qml/Menus/Global/GlobalAudioMenu.qml
        qml/Menus/Global/GlobalFileMenu.qml
        qml/Menus/Global/GlobalHelpMenu.qml
        qml/Menus/Global/GlobalPlaybackMenu.qml
        qml/Menus/Global/GlobalSettingsMenu.qml
        qml/Menus/Global/GlobalSubtitlesMenu.qml
        qml/Menus/Global/GlobalVideoMenu.qml
        qml/Menus/Global/GlobalViewMenu.qml

        qml/Settings/GeneralSettings.qml
        qml/Settings/PlaybackSettings.qml
        qml/Settings/VideoSettings.qml
        qml/Settings/AudioSettings.qml
        qml/Settings/SubtitlesSettings.qml
        qml/Settings/PlaylistSettings.qml
        qml/Settings/MouseSettings.qml
        qml/Settings/ShortcutsSettings.qml
        qml/Settings/CustomCommandsSettings.qml
        qml/Settings/EditCustomCommand.qml
        qml/Settings/DebugSettings.qml
        qml/Settings/SettingsWindow.qml
)

target_include_directories(haruna
    PRIVATE
        models
        models/playlist
        mpv
        ${FFMPEG_INCLUDE_DIRS}
)

target_compile_definitions(haruna PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

target_link_libraries(haruna PRIVATE
    Qt6::Concurrent
    Qt6::Core
    Qt6::Core5Compat
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Sql

    KF6::ColorScheme
    KF6::ConfigCore
    KF6::ConfigGui
    KF6::CoreAddons
    KF6::FileMetaData
    KF6::I18n
    KF6::IconThemes
    KF6::KIOWidgets
    KF6::Kirigami
    KF6::WindowSystem
    KF6::Crash

    KDAB::kdsingleapplication

    MpvQt::MpvQt

    framedecoderplugin
    utilitiesplugin
    settingsplugin
    youtubeplugin
)

if (CMAKE_SYSTEM_NAME IN_LIST DBUS_PLATFORMS)
    target_link_libraries(haruna PRIVATE
        Qt6::DBus
    )
endif()
install(TARGETS haruna DESTINATION ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
