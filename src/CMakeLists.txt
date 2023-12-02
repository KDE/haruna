#
# SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

# DATA_ICONS is defined in data/CMakeLists.txt
ecm_add_app_icon(ICONS_SRCS ICONS ${DATA_ICONS})

add_executable(haruna)

target_sources(haruna
    PRIVATE
    application.h application.cpp
    framedecoder.h framedecoder.cpp
    global.h global.cpp
    KDSingleApplication/kdsingleapplication_localsocket_p.h KDSingleApplication/kdsingleapplication_localsocket.cpp
    KDSingleApplication/kdsingleapplication.h KDSingleApplication/kdsingleapplication.cpp
    main.cpp
    models/actionsmodel.h models/actionsmodel.cpp
    models/chaptersmodel.h models/chaptersmodel.cpp
    models/customcommandsmodel.h models/customcommandsmodel.cpp
    models/recentfilesmodel.h models/recentfilesmodel.cpp
    models/subtitlesfoldersmodel.h models/subtitlesfoldersmodel.cpp
    models/tracksmodel.h models/tracksmodel.cpp
    models/playlistmodel.h models/playlistmodel.cpp
    mpv/mpvitem.h mpv/mpvitem.cpp
    mpv/mpvpreview.h mpv/mpvpreview.cpp
    mpv/mpvproperties.h
    playlistitem.h playlistitem.cpp
    thumbnailimageprovider.h thumbnailimageprovider.cpp
    worker.h worker.cpp
    qml.qrc
    ../help/help.qrc
    ${ICONS_SRCS}
)

if (CMAKE_SYSTEM_NAME IN_LIST X11_PLATFORMS)
    qt_add_dbus_interface(haruna_screensaver_inhibit_SRCS ../org.freedesktop.ScreenSaver.xml screensaverdbusinterface)
    target_sources(haruna
        PRIVATE
        lockmanager.h lockmanager.cpp
        mpris2/mediaplayer2.h mpris2/mediaplayer2.cpp
        mpris2/mediaplayer2player.h mpris2/mediaplayer2player.cpp
        ${haruna_screensaver_inhibit_SRCS}
    )
    target_include_directories(haruna PRIVATE mpris2)
endif()

kconfig_add_kcfg_files(haruna GENERATE_MOC settings/audiosettings.kcfgc)
kconfig_add_kcfg_files(haruna GENERATE_MOC settings/generalsettings.kcfgc)
kconfig_add_kcfg_files(haruna GENERATE_MOC settings/informationsettings.kcfgc)
kconfig_add_kcfg_files(haruna GENERATE_MOC settings/mousesettings.kcfgc)
kconfig_add_kcfg_files(haruna GENERATE_MOC settings/playbacksettings.kcfgc)
kconfig_add_kcfg_files(haruna GENERATE_MOC settings/playlistsettings.kcfgc)
kconfig_add_kcfg_files(haruna GENERATE_MOC settings/subtitlessettings.kcfgc)
kconfig_add_kcfg_files(haruna GENERATE_MOC settings/videosettings.kcfgc)

target_include_directories(haruna
    PRIVATE
        KDSingleApplication
        models
        mpv
        ${FFMPEG_INCLUDE_DIRS}
)

target_compile_definitions(haruna PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

target_link_libraries(haruna PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2

    KF6::ConfigCore
    KF6::ConfigGui
    KF6::CoreAddons
    KF6::DocTools
    KF6::FileMetaData
    KF6::I18n
    KF6::IconThemes
    KF6::KIOWidgets
    KF6::Kirigami2
    KF6::ConfigWidgets
    KF6::WindowSystem

    MpvQt::MpvQt
    ${AVUTIL_LIBRARIES}
    ${AVFILTER_LIBRARIES}
    ${AVFORMAT_LIBRARIES}
    ${AVCODEC_LIBRARIES}
    ${SWSCALE_LIBRARIES}
)

if (CMAKE_SYSTEM_NAME IN_LIST X11_PLATFORMS)
    target_link_libraries(haruna PRIVATE
        Qt6::DBus
    )
endif ()
install(TARGETS haruna DESTINATION ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
