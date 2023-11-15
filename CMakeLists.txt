#
# SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

cmake_minimum_required(VERSION 3.15)

cmake_policy(SET CMP0048 NEW)
project(haruna VERSION 0.12.3 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ECM 5.89 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(FeatureSummary)
include(KDEInstallDirs)
include(ECMInstallIcons)
include(ECMAddAppIcon)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

include(KDEClangFormat)
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h *.hpp *.c)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

include(KDEGitCommitHooks)
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

include(ECMSetupVersion)
ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX HARUNA
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/src/haruna-version.h"
)

set(X11_PLATFORMS Linux FreeBSD GNU/kFreeBSD NetBSD OpenBSD)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2)

find_package(Qt${QT_MAJOR_VERSION}Core)
set_package_properties(Qt${QT_MAJOR_VERSION}Core PROPERTIES TYPE REQUIRED)

find_package(Qt${QT_MAJOR_VERSION}Gui)
set_package_properties(Qt${QT_MAJOR_VERSION}Gui PROPERTIES TYPE REQUIRED)

find_package(Qt${QT_MAJOR_VERSION}Qml)
set_package_properties(Qt${QT_MAJOR_VERSION}Qml PROPERTIES TYPE REQUIRED)

find_package(Qt${QT_MAJOR_VERSION}Quick)
set_package_properties(Qt${QT_MAJOR_VERSION}Quick PROPERTIES TYPE REQUIRED)

find_package(Qt${QT_MAJOR_VERSION}QuickControls2)
set_package_properties(Qt${QT_MAJOR_VERSION}QuickControls2 PROPERTIES TYPE REQUIRED)

if (CMAKE_SYSTEM_NAME IN_LIST X11_PLATFORMS)
    find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS DBus X11Extras)

    find_package(Qt${QT_MAJOR_VERSION}DBus)
    set_package_properties(Qt${QT_MAJOR_VERSION}DBus PROPERTIES TYPE REQUIRED)

    if (QT_MAJOR_VERSION STREQUAL "5")
        find_package(Qt5X11Extras)
        set_package_properties(Qt5X11Extras PROPERTIES TYPE REQUIRED)
    endif ()
endif ()

find_package(Libmpv)
set_package_properties(Libmpv PROPERTIES TYPE REQUIRED
    URL "https://mpv.io")

find_package(FFmpeg)
set_package_properties(FFmpeg PROPERTIES TYPE REQUIRED)

find_package(Breeze)
set_package_properties(Breeze PROPERTIES TYPE RECOMMENDED
    URL "https://api.kde.org/frameworks/breeze-icons/html/index.html")

find_package(KF${QT_MAJOR_VERSION}Config)
set_package_properties(KF${QT_MAJOR_VERSION}Config PROPERTIES TYPE REQUIRED
    URL "https://api.kde.org/frameworks/kconfig/html/index.html")

find_package(KF${QT_MAJOR_VERSION}CoreAddons)
set_package_properties(KF${QT_MAJOR_VERSION}CoreAddons PROPERTIES TYPE REQUIRED
    URL "https://api.kde.org/frameworks/kcoreaddons/html/index.html")

find_package(KF${QT_MAJOR_VERSION}DocTools)
set_package_properties(KF${QT_MAJOR_VERSION}DocTools PROPERTIES
    DESCRIPTION "https://api.kde.org/frameworks/kdoctools/html/index.html")

find_package(KF${QT_MAJOR_VERSION}FileMetaData)
set_package_properties(KF${QT_MAJOR_VERSION}FileMetaData PROPERTIES TYPE REQUIRED
    URL "https://api.kde.org/frameworks/kfilemetadata/html/index.html")

find_package(KF${QT_MAJOR_VERSION}I18n)
set_package_properties(KF${QT_MAJOR_VERSION}I18n PROPERTIES TYPE REQUIRED
    URL "https://api.kde.org/frameworks/ki18n/html/index.html")

find_package(KF${QT_MAJOR_VERSION}IconThemes)
set_package_properties(KF${QT_MAJOR_VERSION}IconThemes PROPERTIES TYPE REQUIRED
    URL "https://api.kde.org/frameworks/kiconthemes/html/index.html")

find_package(KF${QT_MAJOR_VERSION}KIO)
set_package_properties(KF${QT_MAJOR_VERSION}KIO PROPERTIES TYPE REQUIRED
    URL "https://api.kde.org/frameworks/kio/html/index.html")

find_package(KF${QT_MAJOR_VERSION}Kirigami2)
set_package_properties(KF${QT_MAJOR_VERSION}Kirigami2 PROPERTIES TYPE REQUIRED
    URL "https://api.kde.org/frameworks/kirigami/html/index.html")

find_package(KF${QT_MAJOR_VERSION}ConfigWidgets)
set_package_properties(KF${QT_MAJOR_VERSION}ConfigWidgets PROPERTIES TYPE REQUIRED
    URL "https://api.kde.org/frameworks/kconfigwidgets/html/index.html")

find_package(KF${QT_MAJOR_VERSION}WindowSystem)
set_package_properties(KF${QT_MAJOR_VERSION}WindowSystem PROPERTIES TYPE REQUIRED
    URL "https://api.kde.org/frameworks/kwindowsystem/html/index.html")

find_package(YouTubeDl)
set_package_properties(YouTubeDl PROPERTIES TYPE RUNTIME)

find_package(Ytdlp)
set_package_properties(Ytdlp PROPERTIES TYPE RUNTIME)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

ki18n_install(po)

add_subdirectory(data)
add_subdirectory(src)
add_subdirectory(doc)
