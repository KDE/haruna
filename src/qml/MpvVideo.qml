/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import org.kde.haruna 1.0
import org.kde.kirigami 2.10 as Kirigami

MpvItem {
    id: root

    // cache the watch later time position to be used by the seekToWatchLaterPosition action
    // usefull when resuming playback is disabled
    property int watchLaterPosition: -1

    // when playlist repeat is turned off
    // the last file in the playlist is reloaded
    // this property is used to pause the player
    property bool isFileReloaded: false

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
        setProperty("sid", id)
    }

    onSetSecondarySubtitle: {
        setProperty("secondary-sid", id)
    }

    onSetAudio: {
        setProperty("aid", id)
    }

    onReady: {
        if (app.url(0).toString() !== "") {
            window.openFile(app.url(0), true)
        } else {
            if (!PlaybackSettings.openLastPlayedFile) {
                return
            }
            // if both lastPlaylist and lastPlayedFile are set the playlist is loaded
            // and the lastPlayedFile is searched in the playlist
            if (GeneralSettings.lastPlaylist !== "") {
                window.openFile(GeneralSettings.lastPlaylist)
            } else {
                window.openFile(GeneralSettings.lastPlayedFile)
            }
        }
    }

    onFileStarted: {
        if (typeof getProperty("path") === "string" && getProperty("path").startsWith("http")) {
            loadingIndicatorParent.visible = true
        }
    }

    onFileLoaded: {
        if (!getProperty("vid")) {
            command(["video-add", VideoSettings.defaultCover])
        }

        watchLaterPosition = loadTimePosition()

        loadingIndicatorParent.visible = false
        header.audioTracks = getProperty("track-list").filter(track => track["type"] === "audio")
        header.subtitleTracks = getProperty("track-list").filter(track => track["type"] === "sub")

        if (playList.playlistView.count <= 1 && PlaylistSettings.repeat) {
            setProperty("loop-file", "inf")
        }

        setProperty("ab-loop-a", "no")
        setProperty("ab-loop-b", "no")

        if (isFileReloaded) {
            pause = true
            position = 0
            isFileReloaded = false
            return
        }

        if (PlaybackSettings.seekToLastPosition) {
            pause = !PlaybackSettings.playOnResume && watchLaterPosition > 0
            position = watchLaterPosition
        }
    }

    onChapterChanged: {
        if (!PlaybackSettings.skipChapters) {
            return
        }

        const chapters = getProperty("chapter-list")
        const chaptersToSkip = PlaybackSettings.chaptersToSkip
        if (chapters.length === 0 || chaptersToSkip === "") {
            return
        }

        const words = chaptersToSkip.split(",")
        for (let i = 0; i < words.length; ++i) {
            if (chapters[chapter] && chapters[chapter].title.toLowerCase().includes(words[i].trim())) {
                appActions.seekNextChapterAction.trigger()
                if (PlaybackSettings.showOsdOnSkipChapters) {
                    osd.message(i18nc("@info:tooltip", "Skipped chapter: %1", chapters[chapter-1].title))
                }
                // a chapter title can match multiple words
                // return to prevent skipping multiple chapters
                return
            }
        }
    }

    onEndFile: {
        if (reason === "error") {
            if (playlistModel.rowCount() === 0) {
                return
            }

            const item = playlistProxyModel.getItem(playlistProxyModel.getPlayingItem())
            const title = item.mediaTitle() || item.fileName()
            osd.message(i18nc("@info:tooltip", "Could not play: %1", title))
        }
        if (playlistProxyModel.getPlayingItem() + 1 < playList.playlistView.count) {
            appActions.playNextAction.trigger()
        } else {
            // Last file in playlist
            if (PlaylistSettings.repeat) {
                playlistProxyModel.setPlayingItem(0)
                loadFile(playlistProxyModel.getPath(0))
            } else {
                loadFile(playlistProxyModel.getPath())
                isFileReloaded = true
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

            if (!PlaylistSettings.canToggleWithMouse || playList.playlistView.count <= 1) {
                return
            }
            if (PlaylistSettings.position === "right") {
                if (mouseX > width - 50) {
                    playList.state = "visible"
                } else {
                    playList.state = "hidden"
                }
            } else {
                if (mouseX < 50) {
                    playList.state = "visible"
                } else {
                    playList.state = "hidden"
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
