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
    framedecoder.h framedecoder.cpp
    global.h global.cpp
    main.cpp
    models/actionsmodel.h models/actionsmodel.cpp
    models/chaptersmodel.h models/chaptersmodel.cpp
    models/customcommandsmodel.h models/customcommandsmodel.cpp
    models/mouseactionsmodel.h models/mouseactionsmodel.cpp
    models/recentfilesmodel.h models/recentfilesmodel.cpp
    models/subtitlesfoldersmodel.h models/subtitlesfoldersmodel.cpp
    models/tracksmodel.h models/tracksmodel.cpp
    models/playlistmodel.h models/playlistmodel.cpp
    mpv/mpvitem.h mpv/mpvitem.cpp
    mpv/mpvpreview.h mpv/mpvpreview.cpp
    mpv/mpvproperties.h
    thumbnailimageprovider.h thumbnailimageprovider.cpp
    worker.h worker.cpp
    ${ICONS_SRCS}
)

if (CMAKE_SYSTEM_NAME IN_LIST DBUS_PLATFORMS)
    qt_add_dbus_interface(haruna_screensaver_inhibit_SRCS ../org.freedesktop.ScreenSaver.xml screensaverdbusinterface)
    target_sources(haruna
        PRIVATE
        lockmanager.h lockmanager.cpp
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

kconfig_add_kcfg_files(haruna GENERATE_MOC settings/audiosettings.kcfgc)
kconfig_add_kcfg_files(haruna GENERATE_MOC settings/generalsettings.kcfgc)
kconfig_add_kcfg_files(haruna GENERATE_MOC settings/informationsettings.kcfgc)
kconfig_add_kcfg_files(haruna GENERATE_MOC settings/mousesettings.kcfgc)
kconfig_add_kcfg_files(haruna GENERATE_MOC settings/playbacksettings.kcfgc)
kconfig_add_kcfg_files(haruna GENERATE_MOC settings/playlistsettings.kcfgc)
kconfig_add_kcfg_files(haruna GENERATE_MOC settings/subtitlessettings.kcfgc)
kconfig_add_kcfg_files(haruna GENERATE_MOC settings/videosettings.kcfgc)

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
        qml/PlayListItem.qml
        qml/PlayListItemCompact.qml
        qml/PlayListItemWithThumbnail.qml
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
        mpv
        ${FFMPEG_INCLUDE_DIRS}
)

target_compile_definitions(haruna PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

target_link_libraries(haruna PRIVATE
    Qt6::Core
    Qt6::Core5Compat
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2

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
    ${AVUTIL_LIBRARIES}
    ${AVFILTER_LIBRARIES}
    ${AVFORMAT_LIBRARIES}
    ${AVCODEC_LIBRARIES}
    ${SWSCALE_LIBRARIES}
)

if (CMAKE_SYSTEM_NAME IN_LIST DBUS_PLATFORMS)
    target_link_libraries(haruna PRIVATE
        Qt6::DBus
    )
endif()
install(TARGETS haruna DESTINATION ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
