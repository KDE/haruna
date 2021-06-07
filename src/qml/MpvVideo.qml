/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import mpv 1.0

import org.kde.haruna 1.0
import org.kde.kirigami 2.10 as Kirigami

MpvObject {
    id: root

    property int mouseX: mouseArea.mouseX
    property int mouseY: mouseArea.mouseY

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
        setProperty("screenshot-template", VideoSettings.screenshotTemplate)
        setProperty("screenshot-format", VideoSettings.screenshotFormat)
        const preferredAudioTrack = AudioSettings.preferredTrack
        setProperty("aid", preferredAudioTrack === 0 ? "auto" : preferredAudioTrack)
        setProperty("alang", AudioSettings.preferredLanguage)

        const preferredSubTrack = SubtitlesSettings.preferredTrack
        setProperty("sid", preferredSubTrack === 0 ? "auto" : preferredSubTrack)
        setProperty("slang", SubtitlesSettings.preferredLanguage)
        setProperty("sub-file-paths", SubtitlesSettings.subtitlesFolders.join(":"))

        if (app.argument(0) !== "") {
            window.openFile(app.argument(0), true, PlaylistSettings.loadSiblings)
        } else {
            // open last played file
            if (app.isYoutubePlaylist(GeneralSettings.lastPlayedFile)) {
                getYouTubePlaylist(GeneralSettings.lastPlayedFile)
                playList.isYouTubePlaylist = true
            } else {
                // file is local, open normally
                window.openFile(GeneralSettings.lastPlayedFile, false, PlaylistSettings.loadSiblings)
            }
        }
    }

    onYoutubePlaylistLoaded: {
        mpv.command(["loadfile", playlistModel.getPath(GeneralSettings.lastPlaylistIndex)])
        playlistModel.setPlayingVideo(GeneralSettings.lastPlaylistIndex)

        playList.setPlayListScrollPosition()
    }

    onFileStarted: {
        if (typeof getProperty("path") === "string" && getProperty("path").startsWith("http")) {
            loadingIndicatorParent.visible = true
        }
    }

    onFileLoaded: {
        loadingIndicatorParent.visible = false
        header.audioTracks = getProperty("track-list").filter(track => track["type"] === "audio")
        header.subtitleTracks = getProperty("track-list").filter(track => track["type"] === "sub")

        if (playList.playlistView.count <= 1) {
            setProperty("loop-file", "inf")
        }

        setProperty("ab-loop-a", "no")
        setProperty("ab-loop-b", "no")

        mpv.pause = loadTimePosition() !== 0
        position = loadTimePosition()
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
                actions.seekNextChapterAction.trigger()
                if (PlaybackSettings.showOsdOnSkipChapters) {
                    osd.message(qsTr("Skipped chapter: %1").arg(chapters[mpv.chapter-1].title))
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

            const title = playlistModel.getItem(playlistModel.getPlayingVideo()).mediaTitle()
            osd.message(qsTr("Could not play: %1").arg(title))
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
            }
        }
    }

    onPauseChanged: {
        if (pause) {
            footer.playPauseButton.icon.name = "media-playback-start"
            lockManager.setInhibitionOff()
        } else {
            footer.playPauseButton.icon.name = "media-playback-pause"
            lockManager.setInhibitionOn()
        }
    }

    Timer {
        id: saveWatchLaterFileTimer

        interval: 1000
        running: !mpv.pause
        repeat: true

        onTriggered: handleTimePosition()
    }

    Timer {
        id: hideCursorTimer

        property double tx: mouseArea.mouseX
        property double ty: mouseArea.mouseY
        property int timeNotMoved: 0

        running: window.isFullScreen() && mouseArea.containsMouse
        repeat: true
        interval: 50

        onTriggered: {
            if (mouseArea.mouseX === tx && mouseArea.mouseY === ty) {
                if (timeNotMoved > 2000) {
                    app.hideCursor()
                }
            } else {
                app.showCursor()
                timeNotMoved = 0
            }
            tx = mouseArea.mouseX
            ty = mouseArea.mouseY
            timeNotMoved += interval
        }
    }

    MouseArea {
        id: mouseArea

        acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton
        anchors.fill: parent
        hoverEnabled: true

        onPositionChanged: {
            if (!playList.canToggleWithMouse || playList.playlistView.count <= 1) {
                return
            }
            if (playList.position === "right") {
                if (mouseX > width - 50) {
                    playList.state = "visible"
                }
                if (mouseX < width - playList.width - 20) {
                    playList.state = "hidden"
                }
            } else {
                if (mouseX < 50) {
                    playList.state = "visible"
                }
                if (mouseX > playList.width + 20) {
                    playList.state = "hidden"
                }
            }
        }

        onWheel: {
            if (wheel.angleDelta.y > 0) {
                if (MouseSettings.scrollUp) {
                    actions[MouseSettings.scrollUp].trigger()
                }
            } else if (wheel.angleDelta.y) {
                if (MouseSettings.scrollDown) {
                    actions[MouseSettings.scrollDown].trigger()
                }
            }
        }

        onPressed: {
            focus = true
            if (mouse.button === Qt.LeftButton) {
                if (MouseSettings.left) {
                    actions[MouseSettings.left].trigger()
                }
            } else if (mouse.button === Qt.MiddleButton) {
                if (MouseSettings.middle) {
                    actions[MouseSettings.middle].trigger()
                }
            } else if (mouse.button === Qt.RightButton) {
                if (MouseSettings.right) {
                    actions[MouseSettings.right].trigger()
                }
            }
        }

        onDoubleClicked: {
            if (mouse.button === Qt.LeftButton) {
                if (MouseSettings.leftx2) {
                    actions[MouseSettings.leftx2].trigger()
                }
            } else if (mouse.button === Qt.MiddleButton) {
                if (MouseSettings.middlex2) {
                    actions[MouseSettings.middlex2].trigger()
                }
            } else if (mouse.button === Qt.RightButton) {
                if (MouseSettings.rightx2) {
                    actions[MouseSettings.rightx2].trigger()
                }
            }
        }
    }

    DropArea {
        id: dropArea

        property var acceptedSubtitleTypes: ["application/x-subrip", "text/x-ssa"]

        anchors.fill: parent
        keys: ["text/uri-list"]

        onDropped: {
            if (acceptedSubtitleTypes.includes(app.mimeType(drop.urls[0]))) {
                const subFile = drop.urls[0].replace("file://", "")
                command(["sub-add", drop.urls[0], "select"])
            }

            if (app.mimeType(drop.urls[0]).startsWith("video/")) {
                window.openFile(drop.urls[0], true, PlaylistSettings.loadSiblings)
            }
        }
    }

    Connections {
        target: mediaPlayer2Player

        onPlaypause: actions.playPauseAction.trigger()
        onPlay: root.pause = false
        onPause: root.pause = true
        onStop: {
            root.position = 0
            root.pause = true
        }
        onNext: actions.playNextAction.trigger()
        onPrevious: actions.playPreviousAction.trigger()
        onSeek: root.command(["add", "time-pos", offset])
        onOpenUri: openFile(uri, false, false)
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
                running: true
            }

            Component.onCompleted: {
                parent.width = width + 10
                parent.height = height + 10
            }
        }
    }

    Component.onCompleted: {
        mediaPlayer2Player.mpv = root
    }

    function handleTimePosition() {
        if (mpv.position < mpv.duration - 10) {
            saveTimePosition()
        } else {
            resetTimePosition()
        }
    }
}
