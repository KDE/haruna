/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.haruna
import org.kde.kirigami as Kirigami

MpvItem {
    id: root

    property alias mouseY: mouseArea.mouseY


    property int preMinimizePlaybackState: MpvVideo.PlaybackState.Playing
    enum PlaybackState {
        Playing,
        Paused
    }

    signal setSubtitle(int id)
    signal setSecondarySubtitle(int id)
    signal setAudio(int id)

    volume: GeneralSettings.volume

    onSetSubtitle: {
        subtitleId = id
    }

    onSetSecondarySubtitle: {
        secondarySubtitleId = id
    }

    onSetAudio: {
        audioId = id
    }

    onVolumeChanged: {
        osd.message(i18nc("@info:tooltip", "Volume: %1", root.volume))
    }

    onReady: {
        if (app.url(0).toString() !== "") {
            window.openFile(app.url(0), true)
        } else {
            if (PlaybackSettings.openLastPlayedFile) {
                // if both lastPlaylist and lastPlayedFile are set the playlist is loaded
                // and the lastPlayedFile is searched in the playlist
                if (GeneralSettings.lastPlaylist !== "") {
                    window.openFile(GeneralSettings.lastPlaylist)
                } else {
                    window.openFile(GeneralSettings.lastPlayedFile)
                }
            }
        }
        osd.active = true
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

    onOsdMessage: {
        osd.message(text);
    }

    onEndFile: {
        if (reason === "error") {
            if (playlistModel.rowCount() === 0) {
                return
            }

            const item = playlistModel.getItem(playlistProxyModel.getPlayingItem())
            const title = item.mediaTitle() || item.fileName()
            osd.message(i18nc("@info:tooltip", "Could not play: %1", title))
        }
        if (playlistProxyModel.getPlayingItem() + 1 < window.playList.playlistView.count) {
            appActions.playNextAction.trigger()
        } else {
            // Last file in playlist
            if (PlaylistSettings.repeat) {
                playlistProxyModel.setPlayingItem(0)
            } else {
                isFileReloaded = true
                playlistProxyModel.setPlayingItem(playlistProxyModel.getPlayingItem())
            }
        }
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

            if (!PlaylistSettings.canToggleWithMouse || window.playList.playlistView.count <= 1) {
                return
            }
            if (PlaylistSettings.position === "right") {
                if (mouseX > width - 50) {
                    window.playList.state = "visible"
                } else {
                    window.playList.state = "hidden"
                }
            } else {
                if (mouseX < 50) {
                    window.playList.state = "visible"
                } else {
                    window.playList.state = "hidden"
                }
            }
        }

        onWheel: {
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

        onPressed: {
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

        onDoubleClicked: {
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

        onDropped: {
            if (acceptedSubtitleTypes.includes(app.mimeType(drop.urls[0]))) {
                const subFile = drop.urls[0].replace("file://", "")
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
