#
# SPDX-FileCopyrightText: 2025 George Florea Bănuș <georgefb899@gmail.com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

qt_add_library(settings STATIC)
qt_add_qml_module(settings
    URI org.kde.haruna.settings
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/src/org/kde/haruna/settings
    VERSION 1.0
)

# -----------------------------------
# AudioSettings
# -----------------------------------
kconfig_target_kcfg_file(settings
    FILE audiosettings.kcfg
    CLASS_NAME AudioSettings

    MUTATORS
    SINGLETON
    GENERATE_MOC
    DEFAULT_VALUE_GETTERS
    GENERATE_PROPERTIES
    PARENT_IN_CONSTRUCTOR
    QML_REGISTRATION
)

# -----------------------------------
# GeneralSettings
# -----------------------------------
kconfig_target_kcfg_file(settings
    FILE generalsettings.kcfg
    CLASS_NAME GeneralSettings

    MUTATORS
    SINGLETON
    GENERATE_MOC
    DEFAULT_VALUE_GETTERS
    GENERATE_PROPERTIES
    PARENT_IN_CONSTRUCTOR
    QML_REGISTRATION
)

# -----------------------------------
# InformationSettings
# -----------------------------------
kconfig_target_kcfg_file(settings
    FILE informationsettings.kcfg
    CLASS_NAME InformationSettings

    MUTATORS
    SINGLETON
    GENERATE_MOC
    DEFAULT_VALUE_GETTERS
    GENERATE_PROPERTIES
    PARENT_IN_CONSTRUCTOR
    QML_REGISTRATION
)

# -----------------------------------
# MouseSettings
# -----------------------------------
kconfig_target_kcfg_file(settings
    FILE mousesettings.kcfg
    CLASS_NAME MouseSettings

    MUTATORS
    SINGLETON
    GENERATE_MOC
    DEFAULT_VALUE_GETTERS
    GENERATE_PROPERTIES
    PARENT_IN_CONSTRUCTOR
    QML_REGISTRATION
)

# -----------------------------------
# PlaybackSettings
# -----------------------------------
kconfig_target_kcfg_file(settings
    FILE playbacksettings.kcfg
    CLASS_NAME PlaybackSettings

    MUTATORS
    SINGLETON
    GENERATE_MOC
    DEFAULT_VALUE_GETTERS
    GENERATE_PROPERTIES
    PARENT_IN_CONSTRUCTOR
    QML_REGISTRATION
)

# -----------------------------------
# PlaylistSettings
# -----------------------------------
kconfig_target_kcfg_file(settings
    FILE playlistsettings.kcfg
    CLASS_NAME PlaylistSettings

    MUTATORS
    SINGLETON
    GENERATE_MOC
    DEFAULT_VALUE_GETTERS
    GENERATE_PROPERTIES
    PARENT_IN_CONSTRUCTOR
    QML_REGISTRATION
)

# -----------------------------------
# SubtitlesSettings
# -----------------------------------
kconfig_target_kcfg_file(settings
    FILE subtitlessettings.kcfg
    CLASS_NAME SubtitlesSettings

    MUTATORS
    SINGLETON
    GENERATE_MOC
    DEFAULT_VALUE_GETTERS
    GENERATE_PROPERTIES
    PARENT_IN_CONSTRUCTOR
    QML_REGISTRATION
)

# -----------------------------------
# VideoSettings
# -----------------------------------
kconfig_target_kcfg_file(settings
    FILE videosettings.kcfg
    CLASS_NAME VideoSettings

    MUTATORS
    SINGLETON
    GENERATE_MOC
    DEFAULT_VALUE_GETTERS
    GENERATE_PROPERTIES
    PARENT_IN_CONSTRUCTOR
    QML_REGISTRATION
)

target_link_libraries(settings
    PRIVATE
        KF6::ConfigCore
        KF6::ConfigGui
)
