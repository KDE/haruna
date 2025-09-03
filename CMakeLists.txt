#
# SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

cmake_minimum_required(VERSION 3.15)

project(haruna LANGUAGES CXX)
project(haruna VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QT_MIN_VERSION 6.6.0)
set(KF6_MIN_VERSION 6.0.0)

include(FeatureSummary)

find_package(ECM ${KF6_MIN_VERSION} NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED
    URL "https://api.kde.org/ecm/"
    DESCRIPTION "extra cmake modules")
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(Qt6Core ${QT_MIN_VERSION})
set_package_properties(Qt6Core PROPERTIES TYPE REQUIRED)

find_package(Qt6Core5Compat ${QT_MIN_VERSION})
set_package_properties(Qt6Core5Compat PROPERTIES TYPE REQUIRED)

find_package(Qt6Gui ${QT_MIN_VERSION})
set_package_properties(Qt6Gui PROPERTIES TYPE REQUIRED)

find_package(Qt6Qml ${QT_MIN_VERSION})
set_package_properties(Qt6Qml PROPERTIES TYPE REQUIRED)

find_package(Qt6Quick ${QT_MIN_VERSION})
set_package_properties(Qt6Quick PROPERTIES TYPE REQUIRED)

find_package(Qt6QuickControls2 ${QT_MIN_VERSION})
set_package_properties(Qt6QuickControls2 PROPERTIES TYPE REQUIRED)

find_package(MpvQt)
set_package_properties(MpvQt PROPERTIES TYPE REQUIRED
    URL "https://invent.kde.org/libraries/mpvqt")

find_package(FFmpeg)
set_package_properties(FFmpeg PROPERTIES TYPE REQUIRED
    URL "https://ffmpeg.org"
    DESCRIPTION "needed for playlist and mpris thumbnails")

find_package(Breeze)
set_package_properties(Breeze PROPERTIES TYPE RECOMMENDED
    URL "https://api.kde.org/frameworks/breeze-icons/html/index.html")

find_package(KF6ColorScheme ${KF6_MIN_VERSION})
set_package_properties(KF6ColorScheme PROPERTIES TYPE REQUIRED
    URL "https://invent.kde.org/frameworks/kcolorscheme")

find_package(KF6Config ${KF6_MIN_VERSION})
set_package_properties(KF6Config PROPERTIES TYPE REQUIRED
    URL "https://api.kde.org/frameworks/kconfig/html/index.html")

find_package(KF6CoreAddons ${KF6_MIN_VERSION})
set_package_properties(KF6CoreAddons PROPERTIES TYPE REQUIRED
    URL "https://api.kde.org/frameworks/kcoreaddons/html/index.html")

find_package(KF6FileMetaData ${KF6_MIN_VERSION})
set_package_properties(KF6FileMetaData PROPERTIES TYPE REQUIRED
    URL "https://api.kde.org/frameworks/kfilemetadata/html/index.html")

find_package(KF6I18n ${KF6_MIN_VERSION})
set_package_properties(KF6I18n PROPERTIES TYPE REQUIRED
    URL "https://api.kde.org/frameworks/ki18n/html/index.html")

find_package(KF6IconThemes ${KF6_MIN_VERSION})
set_package_properties(KF6IconThemes PROPERTIES TYPE REQUIRED
    URL "https://api.kde.org/frameworks/kiconthemes/html/index.html")

find_package(KF6KIO ${KF6_MIN_VERSION})
set_package_properties(KF6KIO PROPERTIES TYPE REQUIRED
    URL "https://api.kde.org/frameworks/kio/html/index.html")

find_package(KF6Kirigami ${KF6_MIN_VERSION})
set_package_properties(KF6Kirigami PROPERTIES TYPE REQUIRED
    URL "https://api.kde.org/frameworks/kirigami/html/index.html")

find_package(KF6WindowSystem ${KF6_MIN_VERSION})
set_package_properties(KF6WindowSystem PROPERTIES TYPE REQUIRED
    URL "https://api.kde.org/frameworks/kwindowsystem/html/index.html")

find_package(KF6Crash ${KF6_MIN_VERSION})
set_package_properties(KF6Crash PROPERTIES TYPE REQUIRED
    URL "https://api.kde.org/frameworks/kcrash/html/index.html")

find_package(KF6XmlGui ${KF6_MIN_VERSION})
set_package_properties(KF6XmlGui PROPERTIES
    TYPE REQUIRED
    URL "https://api.kde.org/frameworks/kxmlgui/html/index.html")

find_package(Ytdlp)
set_package_properties(Ytdlp PROPERTIES TYPE RUNTIME
    URL "https://github.com/yt-dlp/yt-dlp"
    DESCRIPTION "needed to play videos from youtube and other online sources")


include(KDEInstallDirs)
include(ECMInstallIcons)

add_subdirectory(src/playlist)

add_executable(haruna)
qt_add_dbus_interface(haruna_screensaver_inhibit_SRCS org.freedesktop.ScreenSaver.xml screensaverdbusinterface)
target_sources(haruna
    PRIVATE
        src/main.cpp
        src/application.cpp
        src/settings.cpp
        src/lockmanager.cpp
        src/mpvobject.cpp
        src/playlist/playlistitem.cpp
        src/playlist/playlistmodel.cpp
        src/qthelper.h
        src/subtitlesfoldersmodel.cpp
        src/track.cpp
        src/tracksmodel.cpp
        src/worker.cpp
        src/qml.qrc
        ${haruna_screensaver_inhibit_SRCS}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Core5Compat
    Qt6::Quick
    Qt6::QuickControls2

    KF6::ConfigGui
    KF6::CoreAddons
    KF6::FileMetaData
    KF6::I18n
    KF6::XmlGui

    MpvQt::MpvQt
)

ecm_install_icons(ICONS data/256-apps-com.georgefb.haruna.svg DESTINATION ${KDE_INSTALL_ICONDIR})
install(TARGETS ${PROJECT_NAME} DESTINATION ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES data/com.georgefb.haruna.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(PROGRAMS data/com.georgefb.haruna.desktop DESTINATION ${KDE_INSTALL_APPDIR})

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
