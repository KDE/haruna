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
    main.cpp
    models/actionsmodel.h models/actionsmodel.cpp
    models/customcommandsmodel.h models/customcommandsmodel.cpp
    models/recentfilesmodel.h models/recentfilesmodel.cpp
    models/subtitlesfoldersmodel.h models/subtitlesfoldersmodel.cpp
    models/tracksmodel.h models/tracksmodel.cpp
    models/playlistmodel.h models/playlistmodel.cpp
    mpv/mpvcore.h mpv/mpvcore.cpp
    mpv/mpvitem.h mpv/mpvitem.cpp
    mpv/mpvrenderer.h mpv/mpvrenderer.cpp
    playlistitem.h playlistitem.cpp
    thumbnailimageprovider.h thumbnailimageprovider.cpp
    track.h track.cpp
    worker.h worker.cpp
    qml.qrc
    ../help/help.qrc
    ${ICONS_SRCS}
)

if (CMAKE_SYSTEM_NAME IN_LIST X11_PLATFORMS)
    if (QT_MAJOR_VERSION STREQUAL "5")
        qt5_add_dbus_interface(haruna_screensaver_inhibit_SRCS ../org.freedesktop.ScreenSaver.xml screensaverdbusinterface)
    else()
        qt_add_dbus_interface(haruna_screensaver_inhibit_SRCS ../org.freedesktop.ScreenSaver.xml screensaverdbusinterface)
    endif()
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

target_include_directories(haruna PRIVATE models mpv ${FFMPEG_INCLUDE_DIRS} ${Qt5Gui_PRIVATE_INCLUDE_DIRS})

target_compile_definitions(haruna PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

target_link_libraries(haruna PRIVATE
    Qt${QT_MAJOR_VERSION}::Core
    Qt${QT_MAJOR_VERSION}::Gui
    Qt${QT_MAJOR_VERSION}::Qml
    Qt${QT_MAJOR_VERSION}::Quick
    Qt${QT_MAJOR_VERSION}::QuickControls2

    KF5::ConfigCore
    KF5::ConfigGui
    KF5::CoreAddons
    KF5::DocTools
    KF5::FileMetaData
    KF5::I18n
    KF5::IconThemes
    KF5::KIOWidgets
    KF5::Kirigami2
    KF5::ConfigWidgets

    Libmpv::Libmpv
    ${AVUTIL_LIBRARIES}
    ${AVFILTER_LIBRARIES}
    ${AVFORMAT_LIBRARIES}
    ${AVCODEC_LIBRARIES}
    ${SWSCALE_LIBRARIES}
)

if (CMAKE_SYSTEM_NAME IN_LIST X11_PLATFORMS)
    target_link_libraries(haruna PRIVATE
        Qt${QT_MAJOR_VERSION}::DBus
    )
    if (QT_MAJOR_VERSION STREQUAL "5")
        target_link_libraries(haruna PRIVATE
            Qt5::X11Extras
        )
    endif ()
endif ()
install(TARGETS haruna DESTINATION ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
