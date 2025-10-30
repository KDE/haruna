/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as QQC2
import Qt.labs.platform as Platform

import org.kde.kirigami as Kirigami

import org.kde.haruna
import org.kde.haruna.utilities
import org.kde.haruna.settings

Loader {
    id: root

    required property MpvVideo m_mpv
    required property RecentFilesModel m_recentFilesModel
    required property Loader m_settingsLoader

    property bool isShowAnimationRunning: false
    property bool showGlobalMenu: SystemUtils.platformName() !== "windows"
                                    && Kirigami.Settings.hasPlatformMenuBar
                                    && !Kirigami.Settings.isMobile

    clip: true
    asynchronous: !GeneralSettings.showMenuBar
    state: {
        const mainWindow = Window.window as Main
        if (!mainWindow.isFullScreen() && GeneralSettings.showMenuBar) {
            return "visible"
        } else {
            return "hidden"
        }
    }
    sourceComponent: showGlobalMenu ? globalMenuBarComponent : menuBarComponent

    onVisibleChanged: {
        const mainWindow = Window.window as Main
        mainWindow.resizeWindow()
    }

    Component {
        id: menuBarComponent

        QQC2.MenuBar {
            hoverEnabled: true
            Kirigami.Theme.colorSet: Kirigami.Theme.Header

            FileMenu {
                m_recentFilesModel: root.m_recentFilesModel
            }
            ViewMenu {}
            PlaybackMenu {}
            VideoMenu {}
            SubtitlesMenu {
                m_mpv: root.m_mpv
            }
            AudioMenu {
                m_mpv: root.m_mpv
            }
            SettingsMenu {
                m_settingsLoader: root.m_settingsLoader
            }
            HelpMenu {}
        }
    }

    Component {
        id: globalMenuBarComponent

        Platform.MenuBar {
            GlobalFileMenu {
                m_recentFilesModel: root.m_recentFilesModel
            }
            GlobalViewMenu {}
            GlobalPlaybackMenu {}
            GlobalVideoMenu {}
            GlobalSubtitlesMenu {
                m_mpv: root.m_mpv
            }
            GlobalAudioMenu {
                m_mpv: root.m_mpv
            }
            GlobalSettingsMenu {
                m_settingsLoader: root.m_settingsLoader
            }
            GlobalHelpMenu {}
        }
    }

    states: [
        State {
            name: "hidden"
            PropertyChanges {
                root.height: 0
                root.visible: false
            }
        },
        State {
            name : "visible"
            PropertyChanges {
                root.height: root.implicitHeight
                root.visible: true
            }
        }
    ]

    transitions: [
        Transition {
            from: "visible"
            to: "hidden"

            SequentialAnimation {
                NumberAnimation {
                    target: root
                    property: "height"
                    duration: Kirigami.Units.shortDuration
                    easing.type: Easing.Linear
                }
                PropertyAction {
                    target: root
                    property: "visible"
                    value: false
                }
            }
        },
        Transition {
            from: "hidden"
            to: "visible"

            SequentialAnimation {
                PropertyAction {
                    target: root
                    property: "isShowAnimationRunning"
                    value: true
                }
                PropertyAction {
                    target: root
                    property: "visible"
                    value: true
                }
                NumberAnimation {
                    target: root
                    property: "height"
                    duration: Kirigami.Units.shortDuration
                    easing.type: Easing.Linear
                }
                PropertyAction {
                    target: root
                    property: "isShowAnimationRunning"
                    value: false
                }
            }
        }
    ]
}
