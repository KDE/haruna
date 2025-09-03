/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.0
import QtQuick.Layouts 1.13
import QtQuick.Controls 2.13
import Qt5Compat.GraphicalEffects
import org.kde.kirigami 2.11 as Kirigami

Pane {
    id: root

    signal skipChaptersChanged(bool checked)

    x: -width; y: 0; z: 50
    width: 600
    height: mpv.height
    padding: 10
    state: "hidden"
    hoverEnabled: true

    RowLayout {
        anchors.fill: parent
        Navigation {
            id: nav
            width: root.width * 0.3 - root.padding
            Layout.fillHeight: true
        }

        Rectangle {
            width: 1
            color: Kirigami.Theme.alternateBackgroundColor
            Layout.fillHeight: true
        }

        Flickable {
            id: flick

            clip: true
            height: root.height
            contentHeight: settingsViewLoader.item.contentHeight
            Layout.fillWidth: true
            Layout.fillHeight: true
            ScrollBar.vertical: ScrollBar { id: scrollbar }

            Loader {
                id: settingsViewLoader

                anchors.fill: parent
                sourceComponent: generalSettings
            }
        }
    }

    Component {
        id: generalSettings
        General {
            width: root.width * 0.7 - root.padding
        }
    }
    Component {
        id: colorAdjustmentsSettings
        ColorAdjustments {
            width: root.width * 0.7 - root.padding
        }
    }
    Component {
        id: mouseSettings
        Mouse {
            width: root.width * 0.7 - root.padding
        }
    }
    Component {
        id: playlistSettings
        Playlist {
            width: root.width * 0.7 - root.padding
        }
    }
    Component {
        id: audioSettings
        Audio {
            width: root.width * 0.7 - root.padding
        }
    }
    Component {
        id: subtitlesSettings
        Subtitles {
            width: root.width * 0.7 - root.padding
        }
    }
    Component {
        id: playbackSettings
        Playback {
            width: root.width * 0.7 - root.padding
        }
    }

    states: [
        State {
            name: "hidden"
            PropertyChanges { target: root; x: -width }
            PropertyChanges { target: root; visible: false }
        },
        State {
            name : "visible"
            PropertyChanges { target: root; x: 0 }
            PropertyChanges { target: root; visible: true }
        }
    ]

    transitions: [
        Transition {
            from: "visible"
            to: "hidden"

            SequentialAnimation {
                NumberAnimation {
                    target: root
                    property: "x"
                    duration: 150
                    easing.type: Easing.InQuad
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
                NumberAnimation {
                    target: root
                    property: "x"
                    duration: 150
                    easing.type: Easing.OutQuad
                }
            }
        }
    ]
}
