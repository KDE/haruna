#
# SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

qt5_add_dbus_interface(haruna_screensaver_inhibit_SRCS ../org.freedesktop.ScreenSaver.xml screensaverdbusinterface)

add_executable(haruna)

target_sources(haruna
    PRIVATE
    application.cpp
    haction.cpp
    lockmanager.cpp
    main.cpp
    models/customcommandsmodel.cpp
    models/subtitlesfoldersmodel.cpp
    models/tracksmodel.cpp
    models/playlistmodel.cpp
    mpris2/mediaplayer2.cpp
    mpris2/mediaplayer2player.cpp
    mpvobject.cpp
    qthelper.h
    playlistitem.cpp
    thumbnailimageprovider.cpp
    track.cpp
    worker.cpp
    qml.qrc
    ../help/help.qrc
    ${haruna_screensaver_inhibit_SRCS}
)

kconfig_add_kcfg_files(haruna GENERATE_MOC settings/audiosettings.kcfgc)
kconfig_add_kcfg_files(haruna GENERATE_MOC settings/generalsettings.kcfgc)
kconfig_add_kcfg_files(haruna GENERATE_MOC settings/mousesettings.kcfgc)
kconfig_add_kcfg_files(haruna GENERATE_MOC settings/playbacksettings.kcfgc)
kconfig_add_kcfg_files(haruna GENERATE_MOC settings/playlistsettings.kcfgc)
kconfig_add_kcfg_files(haruna GENERATE_MOC settings/subtitlessettings.kcfgc)
kconfig_add_kcfg_files(haruna GENERATE_MOC settings/videosettings.kcfgc)

target_include_directories(haruna PRIVATE models mpris2)

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
    KF5::XmlGui

    Libmpv::Libmpv
)

install(TARGETS haruna DESTINATION ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
