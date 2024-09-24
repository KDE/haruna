/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.12
import QtQuick.Window 2.12
import mpv 1.0
import AppSettings 1.0
import VideoSettings 1.0

MpvObject {
    id: root

    property int mx
    property int my
    property alias scrollPositionTimer: scrollPositionTimer

    signal setSubtitle(int id)
    signal setSecondarySubtitle(int id)
    signal setAudio(int id)

    width: parent.width
    height: parent.height - footer.height
    anchors.left: settingsEditor.right
    anchors.right: parent.right
    anchors.fill: window.isFullScreen() ? parent : undefined
    volume: AppSettings.volume

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
        const preferredAudioTrack = AppSettings.audioPreferredTrack
        setProperty("aid", preferredAudioTrack === 0 ? "auto" : preferredAudioTrack)
        setProperty("alang", AppSettings.audioPreferredLanguage)

        const preferredSubTrack = AppSettings.subtitlesPreferredTrack
        setProperty("sid", preferredSubTrack === 0 ? "auto" : preferredSubTrack)
        setProperty("slang", AppSettings.subtitlesPreferredLanguage)
        setProperty("sub-file-paths", AppSettings.subtitlesFolders.join(":"))

        if (app.argument(0) !== "") {
            window.openFile(app.argument(0), true, true)
        } else {
            // open last played file, paused and
            // at the position when player was closed or last saved
            window.openFile(AppSettings.lastPlayedFile, false, true)
        }
    }

    onFileLoaded: {
        header.audioTracks = getProperty("track-list").filter(track => track["type"] === "audio")
        header.subtitleTracks = getProperty("track-list").filter(track => track["type"] === "sub")

        if (playList.tableView.rows <= 1) {
            setProperty("loop-file", "inf")
        }

        setProperty("ab-loop-a", "no")
        setProperty("ab-loop-b", "no")
    }

    onChapterChanged: {
        if (!AppSettings.playbackSkipChapters) {
            return
        }

        const chapters = mpv.getProperty("chapter-list")
        const chaptersToSkip = AppSettings.playbackChaptersToSkip
        if (chapters.length === 0 || chaptersToSkip === "") {
            return
        }

        const words = chaptersToSkip.split(",")
        for (let i = 0; i < words.length; ++i) {
            if (chapters[mpv.chapter] && chapters[mpv.chapter].title.toLowerCase().includes(words[i].trim())) {
                actions.seekNextChapterAction.trigger()
                if (AppSettings.playbackShowOsdOnSkipChapters) {
                    osd.message(`Skipped chapter: ${chapters[mpv.chapter].title}`)
                }
                // a chapter title can match multiple words
                // return to prevent skipping multiple chapters
                return
            }
        }
    }

    onEndOfFile: {
        const nextFileRow = playListModel.getPlayingVideo() + 1
        if (nextFileRow < playList.tableView.rows) {
            const nextFile = playListModel.getPath(nextFileRow)
            window.openFile(nextFile, true, false)
            playListModel.setPlayingVideo(nextFileRow)
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
        id: scrollPositionTimer
        interval: 50; running: true; repeat: true

        onTriggered: {
            setPlayListScrollPosition()
            scrollPositionTimer.stop()
        }
    }

    Timer {
        id: saveWatchLaterFileTimer
        interval: 1000
        running: !mpv.pause
        repeat: true

        onTriggered: {
            if (mpv.position < mpv.duration - 10) {
                command(["write-watch-later-config"])
            }
        }
    }

    Timer {
        id: hideCursorTimer

        property int tx: mx
        property int ty: my
        property int timeNotMoved: 0

        interval: 50; running: true; repeat: true

        onTriggered: {
            if (!window.isFullScreen()) {
                return;
            }
            if (mx === tx && my === ty) {
                if (timeNotMoved > 2000) {
                    app.hideCursor()
                }
            } else {
                app.showCursor()
                timeNotMoved = 0
            }
            tx = mx
            ty = my
            timeNotMoved += interval
        }
    }

    MouseArea {
        acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton
        anchors.fill: parent
        hoverEnabled: true

        onEntered: hideCursorTimer.running = true

        onExited: hideCursorTimer.running = false

        onMouseXChanged: {
            focus = true
            mx = mouseX
            if (playList.position === "right") {
                if (mouseX > width - 50 && playList.tableView.rows > 1) {
                    if (playList.canToggleWithMouse) {
                        playList.state = "visible"
                    }
                }
                if (mouseX < width - playList.width) {
                    if (playList.canToggleWithMouse) {
                        playList.state = "hidden"
                    }
                }
            } else {
                if (mouseX < 50 && playList.tableView.rows > 1) {
                    if (playList.canToggleWithMouse) {
                        playList.state = "visible"
                    }
                }
                if (mouseX > playList.width) {
                    if (playList.canToggleWithMouse) {
                        playList.state = "hidden"
                    }
                }
            }
        }

        onMouseYChanged: {
            focus = true
            my = mouseY
        }

        onWheel: function(wheel) {
            if (wheel.angleDelta.y > 0) {
                if (AppSettings.mouseScrollUpAction) {
                    actions.list[AppSettings.mouseScrollUpAction].trigger()
                }
            } else if (wheel.angleDelta.y) {
                if (AppSettings.mouseScrollDownAction) {
                    actions.list[AppSettings.mouseScrollDownAction].trigger()
                }
            }
        }

        onPressed: function(mouse) {
            focus = true
            if (mouse.button === Qt.LeftButton) {
                if (AppSettings.mouseLeftAction) {
                    actions.list[AppSettings.mouseLeftAction].trigger()
                }
            } else if (mouse.button === Qt.MiddleButton) {
                if (AppSettings.mouseMiddleAction) {
                    actions.list[AppSettings.mouseMiddleAction].trigger()
                }
            } else if (mouse.button === Qt.RightButton) {
                if (AppSettings.mouseRightAction) {
                    actions.list[AppSettings.mouseRightAction].trigger()
                }
            }
        }

        onDoubleClicked: function(mouse) {
            if (mouse.button === Qt.LeftButton) {
                if (AppSettings.mouseLeftx2Action) {
                    actions.list[AppSettings.mouseLeftx2Action].trigger()
                }
            } else if (mouse.button === Qt.MiddleButton) {
                if (AppSettings.mouseMiddlex2Action) {
                    actions.list[AppSettings.mouseMiddlex2Action].trigger()
                }
            } else if (mouse.button === Qt.RightButton) {
                if (AppSettings.mouseRightx2Action) {
                    actions.list[AppSettings.mouseRightx2Action].trigger()
                }
            }
        }
    }

    DropArea {
        id: dropArea
        anchors.fill: parent
        keys: ["text/uri-list"]

        onDropped: {
            window.openFile(drop.urls[0], true, true)
        }
    }

    function toggleFullScreen() {
        if (!window.isFullScreen()) {
            settingsEditor.state = "hidden"
            window.showFullScreen()
        } else {
            if (window.preFullScreenVisibility === Window.Windowed) {
                window.showNormal()
            }
            if (window.preFullScreenVisibility == Window.Maximized) {
                window.show()
                window.showMaximized()
            }
        }
        app.showCursor()
        scrollPositionTimer.start()
    }

    function setPlayListScrollPosition() {
        const tableViewRows = playList.tableView.rows
        if (tableViewRows < 1) {
            return;
        }
        playList.tableView.contentY = 0
        const currentItemIndex = playListModel.getPlayingVideo()
        const currentItemPosition = currentItemIndex * playList.rowHeight + currentItemIndex * playList.rowSpacing
        const itemsAfterCurrent = tableViewRows - currentItemIndex
        // height of items bellow the current item
        const heightBellow = itemsAfterCurrent * playList.rowHeight + itemsAfterCurrent * playList.rowSpacing
        const playlistHeight = ((tableViewRows * playList.rowHeight) + (tableViewRows * playList.rowSpacing))
        const isHidden = currentItemPosition > height

        if (isHidden) {
            if (heightBellow > height) {
                playList.tableView.contentY = currentItemPosition
            } else {
                playList.tableView.contentY = playlistHeight - height
            }
        }
    }

}
