#
# SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

cmake_minimum_required(VERSION 3.15)

cmake_policy(SET CMP0048 NEW)
project(haruna VERSION 0.9.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ECM 5.66 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(KDEInstallDirs)
include(ECMInstallIcons)
include(ECMAddAppIcon)
include(ECMSetupVersion)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX HARUNA
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/src/haruna-version.h"
)

set(X11_PLATFORMS Linux FreeBSD GNU/kFreeBSD NetBSD OpenBSD)

find_package(Qt5Core)
set_package_properties(Qt5Core PROPERTIES TYPE REQUIRED)

find_package(Qt5DBus)
set_package_properties(Qt5DBus PROPERTIES TYPE REQUIRED)

find_package(Qt5Gui)
set_package_properties(Qt5Gui PROPERTIES TYPE REQUIRED)

find_package(Qt5Qml)
set_package_properties(Qt5Qml PROPERTIES TYPE REQUIRED)

find_package(Qt5Quick)
set_package_properties(Qt5Quick PROPERTIES TYPE REQUIRED)

find_package(Qt5QuickControls2)
set_package_properties(Qt5QuickControls2 PROPERTIES TYPE REQUIRED)

if (CMAKE_SYSTEM_NAME IN_LIST X11_PLATFORMS)
    find_package(Qt5X11Extras)
    set_package_properties(Qt5X11Extras PROPERTIES TYPE REQUIRED)
endif ()

find_package(Libmpv)
set_package_properties(Libmpv PROPERTIES TYPE REQUIRED
    URL "https://mpv.io")

find_package(FFmpeg)
set_package_properties(FFmpeg PROPERTIES TYPE REQUIRED)

find_package(Breeze)
set_package_properties(Breeze PROPERTIES TYPE RECOMMENDED
    URL "https://api.kde.org/frameworks/breeze-icons/html/index.html")

find_package(KF5Config)
set_package_properties(KF5Config PROPERTIES TYPE REQUIRED
    URL "https://api.kde.org/frameworks/kconfig/html/index.html")

find_package(KF5CoreAddons)
set_package_properties(KF5CoreAddons PROPERTIES TYPE REQUIRED
    URL "https://api.kde.org/frameworks/kcoreaddons/html/index.html")

find_package(KF5DocTools)
set_package_properties(KF5DocTools PROPERTIES
    DESCRIPTION "https://api.kde.org/frameworks/kdoctools/html/index.html")

find_package(KF5FileMetaData)
set_package_properties(KF5FileMetaData PROPERTIES TYPE REQUIRED
    URL "https://api.kde.org/frameworks/kfilemetadata/html/index.html")

find_package(KF5I18n)
set_package_properties(KF5I18n PROPERTIES TYPE REQUIRED
    URL "https://api.kde.org/frameworks/ki18n/html/index.html")

find_package(KF5IconThemes)
set_package_properties(KF5IconThemes PROPERTIES TYPE REQUIRED
    URL "https://api.kde.org/frameworks/kiconthemes/html/index.html")

find_package(KF5KIO)
set_package_properties(KF5KIO PROPERTIES TYPE REQUIRED
    URL "https://api.kde.org/frameworks/kio/html/index.html")

find_package(KF5Kirigami2)
set_package_properties(KF5Kirigami2 PROPERTIES TYPE REQUIRED
    URL "https://api.kde.org/frameworks/kirigami/html/index.html")

find_package(KF5ConfigWidgets)
set_package_properties(KF5ConfigWidgets PROPERTIES TYPE REQUIRED
    URL "https://api.kde.org/frameworks/kconfigwidgets/html/index.html")

find_package(YouTubeDl)
set_package_properties(YouTubeDl PROPERTIES TYPE RUNTIME)

find_package(Ytdlp)
set_package_properties(Ytdlp PROPERTIES TYPE RUNTIME)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

add_subdirectory(data)
add_subdirectory(src)
add_subdirectory(doc)
