/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Window
import QtQuick.Controls

import org.kde.haruna
import org.kde.haruna.settings
import org.kde.kirigami as Kirigami

MpvItem {
    id: root

    property alias mouseY: mouseArea.mouseY


    property int preMinimizePlaybackState: MpvVideo.PlaybackState.Playing
    enum PlaybackState {
        Playing,
        Paused
    }

    onVolumeChanged: {
        osd.message(i18nc("@info:tooltip", "Volume: %1", root.volume))
    }

    onFileStarted: {
        const url = currentUrl.toString()
        if (typeof url === "string" && url.startsWith("http")) {
            loadingIndicatorParent.visible = true
        }
    }

    onFileLoaded: {
        loadingIndicatorParent.visible = false
    }

    onOsdMessage: function(text) {
        osd.message(text);
    }

    onRaise: { app.raiseWindow() }
    onPlayNext: { appActions.playNextAction.trigger() }
    onPlayPrevious: { appActions.playPreviousAction.trigger() }
    onOpenUri: { window.openFile(uri) }

    Timer {
        id: hideCursorTimer

        running: window.isFullScreen() && mouseArea.containsMouse
        repeat: true
        interval: 2000
        onTriggered: mouseArea.hideCursor = true
    }

    MouseArea {
        id: mouseArea

        property bool hideCursor: false

        acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: hideCursor && window.isFullScreen() ? Qt.BlankCursor : Qt.ArrowCursor

        onPositionChanged: {
            hideCursor = false
            hideCursorTimer.restart()

            if (!PlaylistSettings.canToggleWithMouse || playlist.playlistView.count <= 1) {
                return
            }
            if (PlaylistSettings.position === "right") {
                if (mouseX > width - 50) {
                    playlist.state = "visible"
                } else {
                    playlist.state = "hidden"
                }
            } else {
                if (mouseX < 50) {
                    playlist.state = "visible"
                } else {
                    playlist.state = "hidden"
                }
            }
        }

        onWheel: wheel => {
            if (wheel.angleDelta.y > 0) {
                if (MouseSettings.scrollUp) {
                    appActions[MouseSettings.scrollUp].trigger()
                }
            } else if (wheel.angleDelta.y) {
                if (MouseSettings.scrollDown) {
                    appActions[MouseSettings.scrollDown].trigger()
                }
            }
        }

        onPressed: mouse => {
            focus = true
            if (mouse.button === Qt.LeftButton) {
                if (MouseSettings.left) {
                    appActions[MouseSettings.left].trigger()
                }
            } else if (mouse.button === Qt.MiddleButton) {
                if (MouseSettings.middle) {
                    appActions[MouseSettings.middle].trigger()
                }
            } else if (mouse.button === Qt.RightButton) {
                if (MouseSettings.right) {
                    appActions[MouseSettings.right].trigger()
                }
            }
        }

        onDoubleClicked: mouse => {
            if (mouse.button === Qt.LeftButton) {
                if (MouseSettings.leftx2) {
                    appActions[MouseSettings.leftx2].trigger()
                }
            } else if (mouse.button === Qt.MiddleButton) {
                if (MouseSettings.middlex2) {
                    appActions[MouseSettings.middlex2].trigger()
                }
            } else if (mouse.button === Qt.RightButton) {
                if (MouseSettings.rightx2) {
                    appActions[MouseSettings.rightx2].trigger()
                }
            }
        }
    }

    DropArea {
        id: dropArea

        anchors.fill: parent
        keys: ["text/uri-list"]

        onDropped: drop => {
            if (window.acceptedSubtitleTypes.includes(app.mimeType(drop.urls[0]))) {
                command(["sub-add", drop.urls[0], "select"])
            }

            if (app.mimeType(drop.urls[0]).startsWith("video/") || app.mimeType(drop.urls[0]).startsWith("audio/")) {
                window.openFile(drop.urls[0], true)
            }
        }
    }

    Rectangle {
        id: loadingIndicatorParent

        visible: false
        anchors.centerIn: parent
        color: Kirigami.Theme.backgroundColor
        opacity: 0.6

        Kirigami.Icon {
            id: loadingIndicator

            source: "view-refresh"
            anchors.centerIn: parent
            width: Kirigami.Units.iconSizes.large
            height: Kirigami.Units.iconSizes.large

            RotationAnimator {
                target: loadingIndicator
                from: 0
                to: 360
                duration: 1500
                loops: Animation.Infinite
                running: loadingIndicatorParent.visible
            }

            Component.onCompleted: {
                parent.width = width + 10
                parent.height = height + 10
            }
        }
    }
}
