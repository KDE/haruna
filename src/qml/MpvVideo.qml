/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Window

import org.kde.haruna
import org.kde.haruna.settings
import org.kde.kirigami as Kirigami

import org.kde.haruna

MpvItem {
    id: root

    required property Osd osd
    required property MouseActionsModel mouseActionsModel
    property var window: Window.window
    property int preMinimizePlaybackState: MpvVideo.PlaybackState.Playing
    property alias mouseY: mouseArea.mouseY

    signal mousePositionChanged(double x, double y)
    signal openPlaylist()
    signal closePlaylist()

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

    onRaise: { HarunaApp.raiseWindow() }
    onPlayNext: { HarunaApp.actions.playNextAction.trigger() }
    onPlayPrevious: { HarunaApp.actions.playPreviousAction.trigger() }
    onOpenUri: {
        root.window.openFile(uri, RecentFilesModel.OpenedFrom.ExternalApp)
    }

    Timer {
        id: hideCursorTimer

        running: root.window.isFullScreen() && mouseArea.containsMouse

        repeat: true
        interval: 2000
        onTriggered: mouseArea.hideCursor = true
    }

    MouseArea {
        id: mouseArea

        property bool hideCursor: false

        acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton | Qt.ForwardButton | Qt.BackButton
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: hideCursor && root.window.isFullScreen() ? Qt.BlankCursor : Qt.ArrowCursor

        onPositionChanged: {
            root.mousePositionChanged(mouseX, mouseY)

            hideCursor = false
            hideCursorTimer.restart()

            if (!PlaylistSettings.canToggleWithMouse) {
                return
            }
            if (PlaylistSettings.position === "right") {
                if (mouseX > width - 50) {
                    root.openPlaylist()
                } else {
                    root.closePlaylist()
                }
            } else {
                if (mouseX < 50) {
                    root.openPlaylist()
                } else {
                    root.closePlaylist()
                }
            }
        }

        onWheel: function(wheel) {
            const model = root.mouseActionsModel
            let actionName
            let modifier = wheel.modifiers
            if (wheel.angleDelta.y > 0) {
                actionName = model.getAction(MouseActionsModel.ScrollUp, modifier)
            } else if (wheel.angleDelta.y) {
                actionName = model.getAction(MouseActionsModel.ScrollDown, modifier)
            }

            if (actionName) {
                HarunaApp.actions[actionName].trigger()
            }
        }

        onPressed: function(mouse) {
            focus = true
            const model = root.mouseActionsModel
            let actionName
            let modifier = mouse.modifiers
            if (mouse.button === Qt.LeftButton) {
                actionName = model.getAction(Qt.LeftButton, modifier)
            } else if (mouse.button === Qt.MiddleButton) {
                actionName = model.getAction(Qt.MiddleButton, modifier)
            } else if (mouse.button === Qt.RightButton) {
                actionName = model.getAction(Qt.RightButton, modifier)
            } else if (mouse.button === Qt.ForwardButton) {
                actionName = model.getAction(Qt.ForwardButton, modifier)
            } else if (mouse.button === Qt.BackButton) {
                actionName = model.getAction(Qt.BackButton, modifier)
            }

            if (actionName) {
                HarunaApp.actions[actionName].trigger()
            }
        }

        onDoubleClicked: function(mouse) {
            focus = true
            const model = root.mouseActionsModel
            let actionName
            let modifier = mouse.modifiers
            if (mouse.button === Qt.LeftButton) {
                actionName = model.getAction(Qt.LeftButton, modifier, true)
            } else if (mouse.button === Qt.MiddleButton) {
                actionName = model.getAction(Qt.MiddleButton, modifier, true)
            } else if (mouse.button === Qt.RightButton) {
                actionName = model.getAction(Qt.RightButton, modifier, true)
            } else if (mouse.button === Qt.ForwardButton) {
                actionName = model.getAction(Qt.ForwardButton, modifier, true)
            } else if (mouse.button === Qt.BackButton) {
                actionName = model.getAction(Qt.BackButton, modifier, true)
            }

            if (actionName) {
                HarunaApp.actions[actionName].trigger()
            }
        }
    }

    DropArea {
        id: dropArea

        anchors.fill: parent
        keys: ["text/uri-list"]

        onDropped: drop => {
            const mimeType = HarunaApp.mimeType(drop.urls[0])
            if (root.window.acceptedSubtitleTypes.includes(mimeType)) {
                root.commandAsync(["sub-add", drop.urls[0], "select"], MpvItem.AsyncIds.AddSubtitleTrack)
            }

            if (mimeType.startsWith("video/") || mimeType.startsWith("audio/")) {
                root.window.openFile(drop.urls[0], RecentFilesModel.OpenedFrom.ExternalApp)
            }
        }
    }

    Rectangle {
        id: loadingIndicatorParent

        visible: false
        anchors.centerIn: parent
        color: Kirigami.Theme.backgroundColor

        Kirigami.LoadingPlaceholder {
            determinate: false
            anchors.centerIn: parent

            Component.onCompleted: {
                parent.width = width + 20
                parent.height = height + 20
            }
        }
    }
}
