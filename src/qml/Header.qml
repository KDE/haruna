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

        HamburgerMenu {
            visible: !menuBarLoader.visible
            position: HamburgerMenu.Position.Header
        }

        ToolButton {
            action: appActions.openFileAction
            focusPolicy: Qt.NoFocus
        }

        ToolButton {
            action: appActions.openUrlAction
            focusPolicy: Qt.NoFocus
            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.MiddleButton
                onClicked: {
                    openUrlTextField.clear()
                    openUrlTextField.paste()
                    window.openFile(openUrlTextField.text)
                }
            }
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

            Menu {
                id: subtitleMenu

                y: parent.height
                closePolicy: Popup.CloseOnReleaseOutsideParent

                MenuItem { action: appActions.openSubtitlesFileAction }

                Instantiator {
                    id: primarySubtitleMenuInstantiator
                    model: mpv.subtitleTracksModel
                    onObjectAdded: subtitleMenu.addItem( object )
                    onObjectRemoved: subtitleMenu.removeItem( object )
                    delegate: MenuItem {
                        enabled: model.id !== mpv.secondarySubtitleId || model.id === 0
                        checkable: true
                        checked: model.id === mpv.subtitleId
                        text: model.text
                        onTriggered: mpv.subtitleId = model.id
                    }
                }
            }
        }

        ToolButton {
            text: i18nc("@action:intoolbar", "Audio")
            icon.name: "audio-volume-high"
            focusPolicy: Qt.NoFocus

            onReleased: {
                audioMenu.visible = !audioMenu.visible
            }

            Menu {
                id: audioMenu

                y: parent.height
                closePolicy: Popup.CloseOnReleaseOutsideParent

                Instantiator {
                    id: audioMenuInstantiator

                    model: mpv.audioTracksModel
                    onObjectAdded: audioMenu.insertItem( index, object )
                    onObjectRemoved: audioMenu.removeItem( object )
                    delegate: MenuItem {
                        id: audioMenuItem
                        checkable: true
                        checked: model.id === mpv.audioId
                        text: model.text
                        onTriggered: mpv.audioId = model.id
                    }
                }
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
