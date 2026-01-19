/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import Qt5Compat.GraphicalEffects

import org.kde.haruna
import org.kde.haruna.playlist
import org.kde.haruna.utilities
import org.kde.haruna.settings

import org.kde.kirigami as Kirigami

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
            loadingIndicator.visible = true
            loadingIndicator.play = true
        }
    }

    onFileLoaded: {
        loadingIndicator.visible = false
        loadingIndicator.play = false
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

        property bool dragMedia: false
        property bool dragSubtitle: false
        property int subtitleIndex: -1
        property bool mouseOnTopPart: containsDrag && dragMedia && drag.y <= height * 0.25
        property bool mouseOnBottomPart: containsDrag && dragMedia && drag.y > height * 0.25

        anchors.fill: parent
        keys: ["text/uri-list"]

        onEntered: function (drag) {
            for (var i = 0; i < drag.urls.length; ++i) {
                let mimeType = MiscUtils.mimeType(drag.urls[i])
                let isDir = root.defaultFilterProxyModel.isDirectory(drag.urls[i])
                if (mimeType.startsWith("video/") || mimeType.startsWith("audio/") || isDir) {
                    dragMedia = true
                    return
                }
                if (root.window.acceptedSubtitleTypes.includes(mimeType)) {
                    dragSubtitle = true
                    subtitleIndex = i
                }
            }
        }

        onExited: {
            dragMedia = false
            dragSubtitle = false
            subtitleIndex = -1
        }

        onDropped: function (drop) {
            if (dragSubtitle && !dragMedia) {
                // If dragging multiple items and none of them is a media, and if there is a subtitle, load it
                mpv.addSubtitles(drop.urls[subtitleIndex])
                return
            }
            if (mouseOnTopPart) {
                // Append to playlist
                root.defaultFilterProxyModel.addFilesAndFolders(drop.urls, PlaylistModel.Append)
                return
            }
            if (mouseOnBottomPart) {
                // Clear the playlist
                let isDir = root.defaultFilterProxyModel.isDirectory(drop.urls[0])
                if (drop.urls.length > 1 || isDir) {
                    // More than one file/folder dragged. Or at least one folder dragged.
                    root.defaultFilterProxyModel.addFilesAndFolders(drop.urls, PlaylistModel.Clear)
                }
                else {
                    // One file dragged
                    root.window.openFile(drop.urls[0], RecentFilesModel.OpenedFrom.ExternalApp)
                }
                return
            }
        }

        Rectangle {
            id: topDropRect

            visible: dropArea.containsDrag
            color: "transparent"
            width: parent.width
            height: parent.height * 0.25
            radius: Kirigami.Units.cornerRadius

            anchors {
                top: dropArea.top
                left: dropArea.left
                right: dropArea.right
                margins: Kirigami.Units.smallSpacing
            }

            border {
                color: dropArea.mouseOnTopPart
                       ? Kirigami.Theme.activeTextColor
                       : "transparent"
                width: dropArea.mouseOnTopPart ? 3 : 0
            }

            Label {
                anchors.fill: parent
                anchors.centerIn: parent

                text: i18nc("@info:drag&drop append items", "Add to Playlist")
                elide: Text.ElideRight
                color: Kirigami.Theme.textColor
                opacity: dropArea.mouseOnTopPart ? 1 : 0.7

                font.pointSize: dropArea.calculateAdaptiveFontSize(12, 30, 400, 1200)
                font.bold: true
                horizontalAlignment: Qt.AlignCenter
                verticalAlignment: Qt.AlignVCenter
                layer.enabled: true
                layer.effect: DropShadow {
                    cached: true
                    verticalOffset: 1
                    horizontalOffset: 1
                    color: Kirigami.Theme.backgroundColor
                    radius: 12
                    spread: 0.4
                    samples: 20
                }
            }
        }

        Rectangle {
            id: bottomDropRect

            visible: dropArea.containsDrag
            width: parent.width
            color: "transparent"
            radius: Kirigami.Units.cornerRadius

            anchors {
                top: topDropRect.bottom
                left: dropArea.left
                right: dropArea.right
                bottom: dropArea.bottom
                margins: Kirigami.Units.smallSpacing
            }

            border {
                color: dropArea.mouseOnBottomPart
                       ? Kirigami.Theme.activeTextColor
                       : "transparent"
                width: dropArea.mouseOnBottomPart ? 3 : 0
            }

            Label {
                anchors.fill: parent
                anchors.centerIn: parent

                text: i18nc("@info:drag&drop play now", "Play Now")
                elide: Text.ElideRight
                color: Kirigami.Theme.textColor
                opacity: dropArea.mouseOnBottomPart ? 1 : 0.7

                font.pointSize: dropArea.calculateAdaptiveFontSize(18, 36, 400, 1200)
                font.bold: true
                horizontalAlignment: Qt.AlignCenter
                verticalAlignment: Qt.AlignVCenter
                layer.enabled: true
                layer.smooth: true
                layer.effect: DropShadow {
                    cached: true
                    verticalOffset: 1
                    horizontalOffset: 1
                    color: Kirigami.Theme.backgroundColor
                    radius: 12
                    spread: 0.4
                    samples: 20
                }
            }
        }

        function calculateAdaptiveFontSize(minPoint, maxPoint, maxWinHeight, maxWinWidth) {
            let width = root.window.width
            let height = root.window.height
            let cappedWidth = Math.min(Math.max(root.window.minimumWidth, width), maxWinWidth)
            let cappedHeight = Math.min(Math.max(root.window.minimumHeight, height), maxWinHeight)
            const widthScale = maxWinWidth - root.window.minimumWidth
            const heightScale = maxWinHeight - root.window.minimumHeight
            const pointScale = maxPoint - minPoint

            // calculate a font point size separately by considering both current width and height
            let wPoint = (((cappedWidth - root.window.minimumWidth) * pointScale) / widthScale) + minPoint
            let hPoint = (((cappedHeight - root.window.minimumHeight) * pointScale) / heightScale) + minPoint
            return Math.min(wPoint, hPoint)
        }
    }

    Rectangle {
        id: loadingIndicator

        property bool play: false
        property int size: 200

        visible: true
        anchors.centerIn: parent
        radius: Kirigami.Units.cornerRadius
        width: root.width > size ? size : root.width - 10
        height: root.height > size ? size : root.height - 10
        color: play ? Qt.alpha(Kirigami.Theme.backgroundColor, 0.6) : "transparent"

        ColumnLayout {
            anchors.centerIn: parent

            Item {
                property int spriteSize: {
                    const _paddedMaxHeight = root.height - Kirigami.Units.largeSpacing * 2 - loadingText.implicitHeight
                    const _clampedHeight = Math.min(Kirigami.Units.iconSizes.enormous, _paddedMaxHeight)
                    return Math.max(Kirigami.Units.iconSizes.small, _clampedHeight)
                }
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: spriteSize
                Layout.preferredHeight: spriteSize

                AnimatedSprite {
                    anchors.fill: parent
                    currentFrame: 0
                    finishBehavior: AnimatedSprite.FinishAtInitialFrame
                    frameCount: 8
                    // Do NOT use frameRate. It causes flickering. Use frameDuration instead
                    frameDuration: Math.round(1000.0 / 30.0)
                    frameHeight: 192
                    frameWidth: 192
                    interpolate: false
                    source: "qrc:/data/animations/192-haruna-animation-sliding.png"
                    reverse: false
                    running: loadingIndicator.play
                }
            }

            Text {
                id: loadingText

                text: i18nc("@label:title", "Loading…")
                visible: loadingIndicator.play

                font {
                    bold: true
                    pointSize: 16
                }

                Layout.alignment: Qt.AlignHCenter
            }

            layer.enabled: true
            layer.samplerName: "maskSource"
            layer.effect: ShaderEffect {
                property color colorSource: loadingIndicator.play ? Qt.alpha(Kirigami.Theme.textColor, 0.65) : "#222"
                fragmentShader: "qrc:/shader/colorfill.frag.qsb"
                Kirigami.Theme.inherit: false
                Kirigami.Theme.colorSet: Kirigami.Theme.Window
            }
        }
    }
}
