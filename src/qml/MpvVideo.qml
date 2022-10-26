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

    property int mouseX: mouseArea.mouseX
    property int mouseY: mouseArea.mouseY
    // cache the watch later time position to be used by the seekToWatchLaterPosition action
    // usefull when resuming playback is disabled
    property int watchLaterPosition: -1

    // when playlist repeat is turned off
    // the last file in the playlist is reloaded
    // this property is used to pause the player
    property bool isFileReloaded: false

    signal setSubtitle(int id)
    signal setSecondarySubtitle(int id)
    signal setAudio(int id)

    width: parent.width
    height: window.isFullScreen() ? parent.height : parent.height - footer.height
    anchors.left: PlaylistSettings.overlayVideo
                  ? parent.left
                  : (PlaylistSettings.position === "left" ? playList.right : parent.left)
    anchors.right: PlaylistSettings.overlayVideo
                   ? parent.right
                   : (PlaylistSettings.position === "right" ? playList.left : parent.right)
    anchors.top: parent.top
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
        if (app.argument(0) !== "") {
            window.openFile(app.argument(0), true)
        } else {
            if (!PlaybackSettings.openLastPlayedFile) {
                return
            }
            // open last played file
            if (app.isYoutubePlaylist(GeneralSettings.lastPlayedFile)) {
                getYouTubePlaylist(GeneralSettings.lastPlayedFile)
                playList.isYouTubePlaylist = true
            } else {
                // file is local, open normally
                window.openFile(GeneralSettings.lastPlayedFile)
            }
        }
    }

    onYoutubePlaylistLoaded: {
        mpv.command(["loadfile", playlistModel.getPath(GeneralSettings.lastPlaylistIndex)])
        playlistModel.setPlayingVideo(GeneralSettings.lastPlaylistIndex)

        playList.setPlayListScrollPosition()
        recentFilesModel.addUrl(playlistUrl, playlistTitle)
    }

    onFileStarted: {
        if (typeof getProperty("path") === "string" && getProperty("path").startsWith("http")) {
            loadingIndicatorParent.visible = true
        }
    }

    onFileLoaded: {
        root.watchLaterPosition = loadTimePosition()

        loadingIndicatorParent.visible = false
        header.audioTracks = getProperty("track-list").filter(track => track["type"] === "audio")
        header.subtitleTracks = getProperty("track-list").filter(track => track["type"] === "sub")

        if (playList.playlistView.count <= 1 && PlaylistSettings.repeat) {
            setProperty("loop-file", "inf")
        }

        setProperty("ab-loop-a", "no")
        setProperty("ab-loop-b", "no")

        if (isFileReloaded) {
            mpv.pause = true
            position = 0
            isFileReloaded = false
            return
        }

        position = 0
        if (PlaybackSettings.seekToLastPosition) {
            mpv.pause = !PlaybackSettings.playOnResume && root.watchLaterPosition > 0
            position = root.watchLaterPosition
        }
    }

    onChapterChanged: {
        if (!PlaybackSettings.skipChapters) {
            return
        }

        const chapters = mpv.getProperty("chapter-list")
        const chaptersToSkip = PlaybackSettings.chaptersToSkip
        if (chapters.length === 0 || chaptersToSkip === "") {
            return
        }

        const words = chaptersToSkip.split(",")
        for (let i = 0; i < words.length; ++i) {
            if (chapters[mpv.chapter] && chapters[mpv.chapter].title.toLowerCase().includes(words[i].trim())) {
                appActions.seekNextChapterAction.trigger()
                if (PlaybackSettings.showOsdOnSkipChapters) {
                    osd.message(i18n("Skipped chapter: %1", chapters[mpv.chapter-1].title))
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

            const item = playlistModel.getItem(playlistModel.getPlayingVideo())
            const title = item.mediaTitle() || item.fileName()
            osd.message(i18n("Could not play: %1", title))
            // only skip to next video if it's a youtube playList
            // to do: figure out why playback fails and act accordingly
            if (!playList.isYouTubePlaylist) {
                return
            }
        }
        const nextFileRow = playlistModel.getPlayingVideo() + 1
        if (nextFileRow < playList.playlistView.count) {
            const nextFile = playlistModel.getPath(nextFileRow)
            playlistModel.setPlayingVideo(nextFileRow)
            loadFile(nextFile, !playList.isYouTubePlaylist)
        } else {
            // Last file in playlist
            if (PlaylistSettings.repeat) {
                playlistModel.setPlayingVideo(0)
                loadFile(playlistModel.getPath(0), !playList.isYouTubePlaylist)
            } else {
                loadFile(playlistModel.getPath(playlistModel.getPlayingVideo()), !playList.isYouTubePlaylist)
                isFileReloaded = true
            }
        }
    }

    onPlayNext: { appActions.playNextAction.trigger() }
    onPlayPrevious: { appActions.playPreviousAction.trigger() }
    onOpenUri: { window.openFile(uri) }

    Timer {
        id: saveWatchLaterFileTimer

        interval: PlaybackSettings.savePositionInterval * 1000
        running: mpv.duration > 0 && !mpv.pause
        repeat: true

        onTriggered: handleTimePosition()
    }

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
        color: {
            let color = Kirigami.Theme.backgroundColor
            Qt.hsla(color.hslHue, color.hslSaturation, color.hslLightness, 0.2)
        }

        Kirigami.Icon {
            id: loadingIndicator

            source: "view-refresh"
            anchors.centerIn: parent
            width: Kirigami.Units.iconSizes.large
            height: Kirigami.Units.iconSizes.large

            RotationAnimator {
                target: loadingIndicator;
                from: 0;
                to: 360;
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

    function handleTimePosition() {
        // need to check duration > 0 for youtube videos
        if (mpv.duration > 0) {
            if (mpv.position < mpv.duration - 10) {
                saveTimePosition()
            } else {
                resetTimePosition()
            }
        }
    }
}
