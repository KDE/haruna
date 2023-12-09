/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import Qt.labs.platform as Platform

import org.kde.kirigami as Kirigami

import org.kde.haruna

import "Menus"
import "Menus/Global"
import "Settings"

Loader {
    id: root

    property bool showGlobalMenu: app.platformName() !== "windows"
                                    && Kirigami.Settings.hasPlatformMenuBar
                                    && !Kirigami.Settings.isMobile

    state: !window.isFullScreen() && GeneralSettings.showMenuBar ? "visible" : "hidden"
    sourceComponent: showGlobalMenu ? globalMenuBarComponent : menuBarComponent

    onVisibleChanged: {
        window.resizeWindow()
    }

    Component {
        id: menuBarComponent

        QQC2.MenuBar {
            hoverEnabled: true
            Kirigami.Theme.colorSet: Kirigami.Theme.Header

            FileMenu {}
            ViewMenu {}
            PlaybackMenu {}
            VideoMenu {}
            SubtitlesMenu {}
            AudioMenu {}
            SettingsMenu {}
            HelpMenu {}
        }
    }

    Component {
        id: globalMenuBarComponent

        Platform.MenuBar {
            GlobalFileMenu {}
            GlobalViewMenu {}
            GlobalPlaybackMenu {}
            GlobalVideoMenu {}
            GlobalSubtitlesMenu {}
            GlobalAudioMenu {}
            GlobalSettingsMenu {}
            GlobalHelpMenu {}
        }
    }

    states: [
        State {
            name: "hidden"
            PropertyChanges {
                target: root
                height: 0
                opacity: 0
                visible: false
            }
        },
        State {
            name : "visible"
            PropertyChanges {
                target: root
                height: root.implicitHeight
                opacity: 1
                visible: true
            }
        }
    ]

    transitions: [
        Transition {
            from: "visible"
            to: "hidden"

            SequentialAnimation {
                ParallelAnimation {
                    NumberAnimation {
                        target: root
                        property: "opacity"
                        duration: Kirigami.Units.veryShortDuration
                        easing.type: Easing.Linear
                    }
                    NumberAnimation {
                        target: root
                        property: "height"
                        duration: Kirigami.Units.veryShortDuration
                        easing.type: Easing.Linear
                    }
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
                    property: "visible"
                    value: true
                }
                ParallelAnimation {
                    NumberAnimation {
                        target: root
                        property: "height"
                        duration: Kirigami.Units.veryShortDuration
                        easing.type: Easing.Linear
                    }
                    NumberAnimation {
                        target: root
                        property: "opacity"
                        duration: Kirigami.Units.veryShortDuration
                        easing.type: Easing.Linear
                    }
                }
            }
        }
    ]
}
