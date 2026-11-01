#
# SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

# DATA_ICONS is defined in data/CMakeLists.txt
ecm_add_app_icon(ICONS_SRCS ICONS ${DATA_ICONS})

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(haruna)

target_sources(haruna
    PRIVATE
    application.h application.cpp
    lockmanager.h lockmanager.cpp
    main.cpp
    models/actionsmodel.h models/actionsmodel.cpp
    models/chaptersmodel.h models/chaptersmodel.cpp
    models/customcommandsmodel.h models/customcommandsmodel.cpp
    models/fontsmodel.h models/fontsmodel.cpp
    models/mouseactionsmodel.h models/mouseactionsmodel.cpp
    models/recentfilesmodel.h models/recentfilesmodel.cpp
    models/subtitlesfoldersmodel.h models/subtitlesfoldersmodel.cpp
    models/tracksmodel.h models/tracksmodel.cpp
    mpv/mpvitem.h mpv/mpvitem.cpp
    mpv/mpvpreview.h mpv/mpvpreview.cpp
    mpv/mpvproperties.h
    mpv/seekablerangesmodel.h mpv/seekablerangesmodel.cpp
    mprisitem.h mprisitem.cpp
    thumbnailimageprovider.h thumbnailimageprovider.cpp
    worker.h worker.cpp
    ${ICONS_SRCS}
)

ecm_qt_declare_logging_category(haruna
    HEADER "logging/_general.h"
    IDENTIFIER "HarunaGeneral"
    CATEGORY_NAME "org.kde.haruna.general"
    DEFAULT_SEVERITY Info
)

qt_add_resources(haruna animations
    FILES
        ../data/animations/192-haruna-animation-sliding.png
)

qt6_add_shaders(haruna
    URI org.kde.haruna
    VERSION 1.0
    PREFIX
        "/"
    FILES
        shader/colorfill.frag
)

if (CMAKE_SYSTEM_NAME IN_LIST DBUS_PLATFORMS)
    qt_add_dbus_interface(haruna_screensaver_inhibit_SRCS ../org.freedesktop.ScreenSaver.xml screensaverdbusinterface)
    target_sources(haruna
        PRIVATE
        ${haruna_screensaver_inhibit_SRCS}
    )
    target_include_directories(haruna
        PRIVATE
            ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()

add_subdirectory(Database)
add_subdirectory(Playlist)
add_subdirectory(Settings)
add_subdirectory(Transcript)
add_subdirectory(Utilities)
add_subdirectory(YouTube)
add_subdirectory(framedecoder)

set_source_files_properties(qml/Models.qml
    PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

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
        qml/VolumeSlider.qml
        qml/Main.qml
        qml/Models.qml

        qml/Haruna/Components/AudioTracksMenu.qml
        qml/Haruna/Components/ColorPickerButton.qml
        qml/Haruna/Components/HamburgerMenu.qml
        qml/Haruna/Components/ImageAdjustmentSlider.qml
        qml/Haruna/Components/InputPopup.qml
        qml/Haruna/Components/LabelWithTooltip.qml
        qml/Haruna/Components/ResizeablePage.qml
        qml/Haruna/Components/ResizeHandler.qml
        qml/Haruna/Components/SelectActionPopup.qml
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


    DEPENDENCIES
        org.kde.haruna.playlist

    RESOURCES
        ${COMPILED_SHADERS}
)

target_include_directories(haruna
    PRIVATE
        models
        mpv
        ${FFMPEG_INCLUDE_DIRS}
)

target_compile_definitions(haruna PRIVATE $<$<CONFIG:Debug>:QT_QML_DEBUG>)

target_link_libraries(haruna PRIVATE
    Qt6::Core
    Qt6::Core5Compat
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::ShaderTools

    KF6::ColorScheme
    KF6::ConfigCore
    KF6::ConfigGui
    KF6::CoreAddons
    KF6::FileMetaData
    KF6::I18n
    KF6::I18nQml
    KF6::IconThemes
    KF6::KIOWidgets
    KF6::Kirigami
    KF6::WindowSystem
    KF6::Crash

    KDAB::kdsingleapplication

    MpvQt::MpvQt

    Haruna::Database
    Haruna::FrameDecoder
    Haruna::Playlist
    Haruna::Settings
    Haruna::Transcript
    Haruna::Utilities
    Haruna::YouTube
)

if (CMAKE_SYSTEM_NAME IN_LIST DBUS_PLATFORMS)
    add_subdirectory(mpris)
    target_link_libraries(haruna
        PRIVATE
            Haruna::Mpris
    )
endif()
install(TARGETS haruna DESTINATION ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
