/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.13
import QtQuick.Window 2.13
import mpv 1.0

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
    anchors.left: hSettings.right
    anchors.right: parent.right

    onSetSubtitle: {
        if (id !== -1) {
            setProperty("sid", id)
        } else {
            setProperty("sid", "no")
        }
    }

    onSetSecondarySubtitle: {
        if (id !== -1) {
            setProperty("secondary-sid", id)
        } else {
            setProperty("secondary-sid", "no")
        }
    }

    onSetAudio: {
        setProperty("aid", id)
    }

    onReady: {
        const preferredAudioTrack = settings.get("Audio", "PreferredTrack")
        setProperty("aid", preferredAudioTrack === 0 ? "auto" : preferredAudioTrack)
        setProperty("alang", settings.get("Audio", "PreferredLanguage"))

        const preferredSubTrack = settings.get("Audio", "PreferredTrack")
        setProperty("sid", preferredSubTrack === 0 ? "auto" : preferredSubTrack)
        setProperty("slang", settings.get("Subtitle", "PreferredLanguage"))
        setProperty("sub-file-paths", settings.getPath("General", "SubtitlesFolders").join(":"))

        footer.volume.value = settings.get("General", "Volume")
        if (app.argument(0) !== "") {
            window.openFile(app.argument(0), true, true)
        } else {
            // open last played file, paused and
            // at the position when player was closed or last saved
            window.openFile(settings.get("General", "LastPlayedFile"), false, true)
        }
    }

    onFileLoaded: {
        header.audioTracks = getProperty("track-list").filter(track => track["type"] === "audio")
        header.subtitleTracks = getProperty("track-list").filter(track => track["type"] === "sub")
        if (playList.tableView.rows <= 1) {
            setProperty("loop-file", "inf")
        }
    }

    onChapterChanged: {
        if (!settings.get("Playback", "SkipChapters")) {
            return
        }

        const chapters = mpv.getProperty("chapter-list")
        const skipChaptersWords = settings.get("Playback", "SkipChaptersWordList")
        if (chapters.length === 0 || skipChaptersWords === "") {
            return
        }

        const words = skipChaptersWords.split(",")
        for (let i = 0; i < words.length; ++i) {
            if (chapters[mpv.chapter] && chapters[mpv.chapter].title.toLowerCase().includes(words[i].trim())) {
                actions.seekNextChapterAction.trigger()
                if (settings.get("Playback", "ShowOsdOnSkipChapters")) {
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
            if (window.visibility !== Window.FullScreen) {
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
            if (mouseY > window.height - footer.height && window.visibility === Window.FullScreen) {
                footer.visible = true
            } else if (mouseY < window.height - footer.height && window.visibility === Window.FullScreen) {
                footer.visible = false
            }
        }

        onWheel: {
            if (wheel.angleDelta.y > 0) {
                if (settings.get("Mouse", "ScrollUp")) {
                    actions.list[settings.get("Mouse", "ScrollUp")].trigger()
                }
            } else if (wheel.angleDelta.y) {
                if (settings.get("Mouse", "ScrollDown")) {
                    actions.list[settings.get("Mouse", "ScrollDown")].trigger()
                }
            }
        }

        onPressed: function(mouse) {
            focus = true
            if (mouse.button === Qt.LeftButton) {
                if (settings.get("Mouse", "Left")) {
                    actions.list[settings.get("Mouse", "Left")].trigger()
                }
            } else if (mouse.button === Qt.MiddleButton) {
                if (settings.get("Mouse", "Middle")) {
                    actions.list[settings.get("Mouse", "Middle")].trigger()
                }
            } else if (mouse.button === Qt.RightButton) {
                if (settings.get("Mouse", "Right")) {
                    actions.list[settings.get("Mouse", "Right")].trigger()
                }
            }
        }

        onDoubleClicked: {
            if (mouse.button === Qt.LeftButton) {
                if (settings.get("Mouse", "Left.x2")) {
                    actions.list[settings.get("Mouse", "Left.x2")].trigger()
                }
            } else if (mouse.button === Qt.MiddleButton) {
                if (settings.get("Mouse", "Middle.x2")) {
                    actions.list[settings.get("Mouse", "Middle.x2")].trigger()
                }
            } else if (mouse.button === Qt.RightButton) {
                if (settings.get("Mouse", "Right.x2")) {
                    actions.list[settings.get("Mouse", "Right.x2")].trigger()
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
        if (window.visibility !== Window.FullScreen) {
            hSettings.state = "hidden"
            window.showFullScreen()
            menuBar.visible = false
            header.visible = false
            footer.visible = false
            footer.anchors.bottom = bottom
            anchors.fill = parent
        } else {
            if (window.preFullScreenVisibility === Window.Windowed) {
                window.showNormal()
            }
            if (window.preFullScreenVisibility == Window.Maximized) {
                window.show()
                window.showMaximized()
            }
            menuBar.visible = settings.get("View", "MenuBarVisible")
            header.visible = settings.get("View", "HeaderVisible")
            footer.visible = true
            footer.anchors.bottom = undefined
            anchors.fill = undefined
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
