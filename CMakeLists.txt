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

set(QT_MIN_VERSION 6.6.0)
set(KF6_MIN_VERSION 6.0.0)

find_package(ECM ${KF6_MIN_VERSION} REQUIRED NO_MODULE)
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

set(DBUS_PLATFORMS Linux FreeBSD GNU/kFreeBSD NetBSD OpenBSD)

find_package(Qt6Core ${QT_MIN_VERSION})
set_package_properties(Qt6Core PROPERTIES TYPE REQUIRED)

find_package(Qt6Gui ${QT_MIN_VERSION})
set_package_properties(Qt6Gui PROPERTIES TYPE REQUIRED)

find_package(Qt6Qml ${QT_MIN_VERSION})
set_package_properties(Qt6Qml PROPERTIES TYPE REQUIRED)

find_package(Qt6Quick ${QT_MIN_VERSION})
set_package_properties(Qt6Quick PROPERTIES TYPE REQUIRED)

find_package(Qt6QuickControls2 ${QT_MIN_VERSION})
set_package_properties(Qt6QuickControls2 PROPERTIES TYPE REQUIRED)

if (CMAKE_SYSTEM_NAME IN_LIST DBUS_PLATFORMS)
    find_package(Qt6DBus ${QT_MIN_VERSION})
    set_package_properties(Qt6DBus PROPERTIES TYPE REQUIRED)
endif ()

find_package(MpvQt)
set_package_properties(MpvQt PROPERTIES TYPE REQUIRED
    URL "https://invent.kde.org/libraries/mpvqt")

find_package(FFmpeg)
set_package_properties(FFmpeg PROPERTIES TYPE REQUIRED)

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

find_package(KF6DocTools ${KF6_MIN_VERSION})
set_package_properties(KF6DocTools PROPERTIES
    DESCRIPTION "https://api.kde.org/frameworks/kdoctools/html/index.html")

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

find_package(YouTubeDl)
set_package_properties(YouTubeDl PROPERTIES TYPE RUNTIME)

find_package(Ytdlp)
set_package_properties(Ytdlp PROPERTIES TYPE RUNTIME)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

ki18n_install(po)

add_subdirectory(data)
add_subdirectory(src)
add_subdirectory(doc)
