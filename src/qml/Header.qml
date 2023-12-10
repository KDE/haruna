/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQml

import org.kde.kirigami as Kirigami
import org.kde.haruna

import "Menus"
import "Haruna/Components"

ToolBar {
    id: root

    position: ToolBar.Header
    state: !window.isFullScreen() && GeneralSettings.showHeader ? "visible" : "hidden"

    onVisibleChanged: {
        window.resizeWindow()
    }

    RowLayout {
        id: headerRow

        width: parent.width

        Loader {
            asynchronous: true
            visible: menuBarLoader.state === "hidden"
            sourceComponent: HamburgerMenu {
                position: HamburgerMenu.Position.Header
            }
        }

        ToolButton {
            action: appActions.openFileAction
            focusPolicy: Qt.NoFocus
        }

        ToolButton {
            action: appActions.openUrlAction
            focusPolicy: Qt.NoFocus
        }

        ToolSeparator {
            padding: vertical ? 10 : 2
            topPadding: vertical ? 2 : 10
            bottomPadding: vertical ? 2 : 10

            contentItem: Rectangle {
                implicitWidth: parent.vertical ? 1 : 24
                implicitHeight: parent.vertical ? 24 : 1
                color: Kirigami.Theme.textColor
            }
        }

        ToolButton {
            id: subtitleMenuButton

            text: i18nc("@action:intoolbar", "Subtitles")
            icon.name: "add-subtitle"
            focusPolicy: Qt.NoFocus

            onReleased: {
                subtitleMenu.visible = !subtitleMenu.visible
            }

            SubtitleTracksMenu {
                id: subtitleMenu

                y: parent.height
                model: mpv.subtitleTracksModel
                isPrimarySubtitleMenu: true
            }
        }

        ToolButton {
            text: i18nc("@action:intoolbar", "Audio")
            icon.name: "audio-volume-high"
            focusPolicy: Qt.NoFocus

            onReleased: {
                audioMenu.visible = !audioMenu.visible
            }

            AudioTracksMenu {
                id: audioMenu

                y: parent.height
                model: mpv.audioTracksModel
            }
        }

        Item {
            Layout.fillWidth: true
        }

        ToolButton {
            action: appActions.configureAction
            focusPolicy: Qt.NoFocus
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
