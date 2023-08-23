/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import org.kde.kirigami 2.11 as Kirigami
import org.kde.haruna 1.0

import "Menus"

ToolBar {
    id: root

    state: !window.isFullScreen() && GeneralSettings.showHeader ? "visible" : "hidden"

    RowLayout {
        anchors.fill: parent

        Kirigami.ActionToolBar {
            actions: [
                hamburgerMenuAction,
                appActions.openFileAction,
                appActions.openUrlAction,
                subtitlesMenuAction,
                audioMenuAction,
            ]
        }

        Kirigami.ActionToolBar {
            actions: [
                appActions.configureAction
            ]
            Layout.preferredWidth: maximumContentWidth
            Layout.fillWidth: false
        }

        Kirigami.Action {
            id: hamburgerMenuAction

            visible: !menuBarLoader.visible
            displayComponent: HamburgerMenu {
                position: HamburgerMenu.Position.Header
            }
        }

        Kirigami.Action {
            id: subtitlesMenuAction

            text: i18nc("@action:intoolbar", "Subtitles")
            icon.name: "add-subtitle"
            displayComponent: ToolButton {
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
        }

        Kirigami.Action {
            id: audioMenuAction

            text: i18nc("@action:intoolbar", "Audio")
            icon.name: "add-subtitle"
            displayComponent: ToolButton {
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
        }
    } // RowLayout

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
