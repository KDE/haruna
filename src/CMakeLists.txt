#
# SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

qt5_add_dbus_interface(haruna_screensaver_inhibit_SRCS ../org.freedesktop.ScreenSaver.xml screensaverdbusinterface)

# DATA_ICONS is defined in data/CMakeLists.txt
ecm_add_app_icon(ICONS_SRCS ICONS ${DATA_ICONS})

add_executable(haruna)

target_sources(haruna
    PRIVATE
    actionsmanager.h actionsmanager.cpp
    application.h application.cpp
    framedecoder.h framedecoder.cpp
    global.h global.cpp
    haction.h haction.cpp
    lockmanager.h lockmanager.cpp
    main.cpp
    models/customcommandsmodel.h models/customcommandsmodel.cpp
    models/recentfilesmodel.h models/recentfilesmodel.cpp
    models/subtitlesfoldersmodel.h models/subtitlesfoldersmodel.cpp
    models/tracksmodel.h models/tracksmodel.cpp
    models/playlistmodel.h models/playlistmodel.cpp
    mpris2/mediaplayer2.h mpris2/mediaplayer2.cpp
    mpris2/mediaplayer2player.h mpris2/mediaplayer2player.cpp
    mpv/mpvobject.h mpv/mpvobject.cpp
    mpv/mpvqthelper.h
    playlistitem.h playlistitem.cpp
    thumbnailimageprovider.h thumbnailimageprovider.cpp
    track.h track.cpp
    worker.h worker.cpp
    qml.qrc
    ../help/help.qrc
    ${haruna_screensaver_inhibit_SRCS}
    ${ICONS_SRCS}
)

kconfig_add_kcfg_files(haruna GENERATE_MOC settings/audiosettings.kcfgc)
kconfig_add_kcfg_files(haruna GENERATE_MOC settings/generalsettings.kcfgc)
kconfig_add_kcfg_files(haruna GENERATE_MOC settings/mousesettings.kcfgc)
kconfig_add_kcfg_files(haruna GENERATE_MOC settings/playbacksettings.kcfgc)
kconfig_add_kcfg_files(haruna GENERATE_MOC settings/playlistsettings.kcfgc)
kconfig_add_kcfg_files(haruna GENERATE_MOC settings/subtitlessettings.kcfgc)
kconfig_add_kcfg_files(haruna GENERATE_MOC settings/videosettings.kcfgc)

target_include_directories(haruna PRIVATE models mpris2 mpv ${FFMPEG_INCLUDE_DIRS})

target_compile_definitions(haruna PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

target_link_libraries(haruna PRIVATE
    Qt5::Core
    Qt5::DBus
    Qt5::Qml
    Qt5::Quick
    Qt5::QuickControls2

    KF5::ConfigCore
    KF5::ConfigGui
    KF5::CoreAddons
    KF5::DocTools
    KF5::FileMetaData
    KF5::I18n
    KF5::IconThemes
    KF5::KIOCore
    KF5::KIOFileWidgets
    KF5::KIOWidgets
    KF5::KIONTLM
    KF5::Kirigami2
    KF5::ConfigWidgets
    KF5::XmlGui

    Libmpv::Libmpv
    ${AVUTIL_LIBRARIES}
    ${AVFILTER_LIBRARIES}
    ${AVFORMAT_LIBRARIES}
    ${AVCODEC_LIBRARIES}
    ${SWSCALE_LIBRARIES}
)

install(TARGETS haruna DESTINATION ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
