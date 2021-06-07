/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.12

import org.kde.kirigami 2.11 as Kirigami
import org.kde.haruna 1.0

Rectangle {
    id: root

    property alias scrollPositionTimer: scrollPositionTimer
    property alias playlistView: playlistView
    property bool canToggleWithMouse: PlaylistSettings.canToggleWithMouse
    property string position: PlaylistSettings.position
    property int rowHeight: PlaylistSettings.rowHeight
    property int bigFont: PlaylistSettings.bigFontFullscreen
    property bool isYouTubePlaylist: false

    height: mpv.height
    width: {
        if (PlaylistSettings.style === "compact") {
            return Kirigami.Units.gridUnit * 20
        } else {
            const w = Kirigami.Units.gridUnit * 30
            return (parent.width * 0.33) < w ? w : parent.width * 0.33
        }
    }
    x: position === "right" ? parent.width : -width
    y: 0
    state: "hidden"
    color: Kirigami.Theme.backgroundColor

    ScrollView {
        id: playlistScrollView

        z: 20
        anchors.fill: parent
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

        ListView {
            id: playlistView

            model: mpv.playlistModel
            spacing: 1
            delegate: {
                switch (PlaylistSettings.style) {
                case "default":
                    playListItemSimple
                    break
                case "withThumbnails":
                    playListItemWithThumbnail
                    break
                case "compact":
                    playListItemCompact
                    break
                }
            }
        }
    }

    Component {
        id: playListItemWithThumbnail
        PlayListItemWithThumbnail {}
    }

    Component {
        id: playListItemSimple
        PlayListItem {}
    }

    Component {
        id: playListItemCompact
        PlayListItemCompact {}
    }

    Timer {
        id: scrollPositionTimer
        interval: 50; running: true; repeat: true

        onTriggered: {
            setPlayListScrollPosition()
            scrollPositionTimer.stop()
        }
    }

    ShaderEffectSource {
        id: shaderEffect

        visible: PlaylistSettings.overlayVideo
        anchors.fill: playlistScrollView
        sourceItem: mpv
        sourceRect: position === "right"
                    ? Qt.rect(mpv.width - root.width, mpv.y, root.width, root.height)
                    : Qt.rect(0, 0, root.width, root.height)
    }

    FastBlur {
        visible: PlaylistSettings.overlayVideo
        anchors.fill: shaderEffect
        radius: 100
        source: shaderEffect
        z: 10
    }

    states: [
        State {
            name: "hidden"
            PropertyChanges { target: root; x: position === "right" ? parent.width : -width }
            PropertyChanges { target: root; visible: false }
        },
        State {
            name : "visible"
            PropertyChanges { target: root; x: position === "right" ? parent.width - root.width : 0 }
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
                    duration: 120
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
                    duration: 120
                    easing.type: Easing.OutQuad
                }
            }
        }
    ]

    function setPlayListScrollPosition() {
        playlistView.positionViewAtIndex(playlistView.model.playingVideo, ListView.Beginning)
    }

}
