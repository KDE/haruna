/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

import org.kde.haruna 1.0

QtObject {
    id: root

    property var list: ({})

    property Action openContextMenuAction: Action {
        id: openContextMenuAction
        property var qaction: app.action("openContextMenu")
        text: qaction.text
        shortcut: qaction.shortcutName()
        icon.name: qaction.iconName()

        Component.onCompleted: list["openContextMenuAction"] = openContextMenuAction

        onTriggered: mpvContextMenu.popup()
    }

    property Action togglePlaylistAction: Action {
        id: togglePlaylistAction
        property var qaction: app.action("togglePlaylist")
        text: qaction.text
        shortcut: qaction.shortcutName()
        icon.name: qaction.iconName()

        Component.onCompleted: list["togglePlaylistAction"] = togglePlaylistAction

        onTriggered: {
            if (playList.state === "visible") {
                playList.state = "hidden"
            } else {
                playList.state = "visible"
            }
        }
    }

    property Action volumeUpAction: Action {
        id: volumeUpAction
        property var qaction: app.action("volumeUp")
        text: qaction.text
        shortcut: qaction.shortcutName()
        icon.name: qaction.iconName()

        Component.onCompleted: list["volumeUpAction"] = volumeUpAction

        onTriggered: {
            mpv.command(["add", "volume", GeneralSettings.volumeStep])
            osd.message(`Volume: ${parseInt(mpv.getProperty("volume"))}`)
        }
    }

    property Action volumeDownAction: Action {
        id: volumeDownAction
        property var qaction: app.action("volumeDown")
        text: qaction.text
        shortcut: qaction.shortcutName()
        icon.name: qaction.iconName()

        Component.onCompleted: list["volumeDownAction"] = volumeDownAction

        onTriggered: {
            mpv.command(["add", "volume", -GeneralSettings.volumeStep])
            osd.message(`Volume: ${parseInt(mpv.getProperty("volume"))}`)
        }
    }

    property Action muteAction: Action {
        id: muteAction
        property var qaction: app.action("mute")
        text: qaction.text
        shortcut: qaction.shortcutName()
        icon.name: qaction.iconName()

        Component.onCompleted: list["muteAction"] = muteAction

        onTriggered: {
            mpv.setProperty("mute", !mpv.getProperty("mute"))
            if (mpv.getProperty("mute")) {
                text = qsTr("Unmute")
                icon.name = "player-volume-muted"
            } else {
                text = qaction.text
                icon.name = qaction.iconName()
            }
        }
    }

    property Action playNextAction: Action {
        id: playNextAction
        property var qaction: app.action("playNext")
        text: qaction.text
        shortcut: qaction.shortcutName()
        icon.name: qaction.iconName()

        Component.onCompleted: list["playNextAction"] = playNextAction

        onTriggered: {
            const nextFileRow = mpv.playlistModel.getPlayingVideo() + 1
            const updateLastPlayedFile = !playList.isYouTubePlaylist
            if (nextFileRow < playList.playlistView.count) {
                const nextFile = mpv.playlistModel.getPath(nextFileRow)
                mpv.playlistModel.setPlayingVideo(nextFileRow)
                mpv.loadFile(nextFile, updateLastPlayedFile)
            } else {
                // Last file in playlist
                if (PlaylistSettings.repeat) {
                    mpv.playlistModel.setPlayingVideo(0)
                    mpv.loadFile(mpv.playlistModel.getPath(0), updateLastPlayedFile)
                }
            }
        }
    }

    property Action playPreviousAction: Action {
        id: playPreviousAction
        property var qaction: app.action("playPrevious")
        text: qaction.text
        shortcut: qaction.shortcutName()
        icon.name: qaction.iconName()

        Component.onCompleted: list["playPreviousAction"] = playPreviousAction

        onTriggered: {
            if (mpv.playlistModel.getPlayingVideo() !== 0) {
                const previousFileRow = mpv.playlistModel.getPlayingVideo() - 1
                const previousFile = mpv.playlistModel.getPath(previousFileRow)
                const updateLastPlayedFile = !playList.isYouTubePlaylist
                mpv.playlistModel.setPlayingVideo(previousFileRow)
                mpv.loadFile(previousFile, updateLastPlayedFile)
            }
        }
    }

    property Action openAction: Action {
        id: openAction
        property var qaction: app.action("openFile")
        text: qaction.text
        shortcut: qaction.shortcutName()
        icon.name: qaction.iconName()

        Component.onCompleted: list["openAction"] = openAction

        onTriggered: fileDialog.open()
    }

    property Action openUrlAction: Action {
        id: openUrlAction
        property var qaction: app.action("openUrl")
        text: qaction.text
        shortcut: qaction.shortcutName()
        icon.name: qaction.iconName()

        Component.onCompleted: list["openUrlAction"] = openUrlAction

        onTriggered: {
            if (openUrlPopup.visible) {
                openUrlPopup.close()
            } else {
                openUrlPopup.open()
            }
        }
    }

    property Action aboutHarunaAction: Action {
        id: aboutHarunaAction
        property var qaction: app.action("aboutHaruna")
        text: qaction.text
        shortcut: qaction.shortcutName()
        icon.name: qaction.iconName()

        Component.onCompleted: list["aboutHarunaAction"] = aboutHarunaAction

        onTriggered: qaction.trigger()
    }

    property Action seekForwardSmallAction: Action {
        id: seekForwardSmallAction
        property var qaction: app.action("seekForwardSmall")
        text: qaction.text
        shortcut: qaction.shortcutName()
        icon.name: qaction.iconName()

        Component.onCompleted: list["seekForwardSmallAction"] = seekForwardSmallAction

        onTriggered: mpv.command(["seek", GeneralSettings.seekSmallStep, "exact"])
    }

    property Action seekBackwardSmallAction: Action {
        id: seekBackwardSmallAction
        property var qaction: app.action("seekBackwardSmall")
        text: qaction.text
        shortcut: qaction.shortcutName()
        icon.name: qaction.iconName()

        Component.onCompleted: list["seekBackwardSmallAction"] = seekBackwardSmallAction

        onTriggered: mpv.command(["seek", -GeneralSettings.seekSmallStep, "exact"])
    }

    property Action seekForwardMediumAction: Action {
        id: seekForwardMediumAction
        property var qaction: app.action("seekForwardMedium")
        text: qaction.text
        shortcut: qaction.shortcutName()
        icon.name: qaction.iconName()

        Component.onCompleted: list["seekForwardMediumAction"] = seekForwardMediumAction

        onTriggered: mpv.command(["seek", GeneralSettings.seekMediumStep, "exact"])
    }

    property Action seekBackwardMediumAction: Action {
        id: seekBackwardMediumAction
        property var qaction: app.action("seekBackwardMedium")
        text: qaction.text
        shortcut: qaction.shortcutName()
        icon.name: qaction.iconName()

        Component.onCompleted: list["seekBackwardMediumAction"] = seekBackwardMediumAction

        onTriggered: mpv.command(["seek", -GeneralSettings.seekMediumStep, "exact"])
    }

    property Action seekForwardBigAction: Action {
        id: seekForwardBigAction
        property var qaction: app.action("seekForwardBig")
        text: qaction.text
        shortcut: qaction.shortcutName()
        icon.name: qaction.iconName()

        Component.onCompleted: list["seekForwardBigAction"] = seekForwardBigAction

        onTriggered: mpv.command(["seek", GeneralSettings.seekBigStep, "exact"])
    }

    property Action seekBackwardBigAction: Action {
        id: seekBackwardBigAction
        property var qaction: app.action("seekBackwardBig")
        text: qaction.text
        shortcut: qaction.shortcutName()
        icon.name: qaction.iconName()

        Component.onCompleted: list["seekBackwardBigAction"] = seekBackwardBigAction

        onTriggered: mpv.command(["seek", -GeneralSettings.seekBigStep, "exact"])
    }

    property Action seekPreviousChapterAction: Action {
        id: seekPreviousChapterAction
        property var qaction: app.action("seekPreviousChapter")
        text: qaction.text
        shortcut: qaction.shortcutName()
        icon.name: qaction.iconName()

        Component.onCompleted: list["seekPreviousChapterAction"] = seekPreviousChapterAction

        onTriggered: {
            mpv.command(["add", "chapter", "-1"])
        }
    }

    property Action seekNextChapterAction: Action {
        id: seekNextChapterAction
        property var qaction: app.action("seekNextChapter")
        text: qaction.text
        shortcut: qaction.shortcutName()
        icon.name: qaction.iconName()

        Component.onCompleted: list["seekNextChapterAction"] = seekNextChapterAction

        onTriggered: {
            const chapters = mpv.getProperty("chapter-list")
            const currentChapter = mpv.getProperty("chapter")
            const nextChapter = currentChapter + 1
            if (nextChapter === chapters.length) {
                playNextAction.trigger()
                return
            }
            mpv.command(["add", "chapter", "1"])
        }
    }

    property Action seekNextSubtitleAction: Action {
        id: seekNextSubtitleAction
        property var qaction: app.action("seekNextSubtitle")
        text: qaction.text
        shortcut: qaction.shortcutName()
        icon.name: qaction.iconName()

        Component.onCompleted: list["seekNextSubtitleAction"] = seekNextSubtitleAction

        onTriggered: {
            if (mpv.getProperty("sid") !== false) {
                mpv.command(["sub-seek", "1"])
            } else {
                seekForwardSmallAction.trigger()
            }
        }
    }

    property Action seekPrevSubtitleAction: Action {
        id: seekPrevSubtitleAction
        property var qaction: app.action("seekPreviousSubtitle")
        text: qaction.text
        shortcut: qaction.shortcutName()
        icon.name: qaction.iconName()

        Component.onCompleted: list["seekPrevSubtitleAction"] = seekPrevSubtitleAction

        onTriggered: {
             if (mpv.getProperty("sid") !== false) {
                 mpv.command(["sub-seek", "-1"])
             } else {
                 seekBackwardSmallAction.trigger()
             }
         }
    }

    property Action frameStepAction: Action {
        id: frameStepAction
        property var qaction: app.action("frameStep")
        text: qaction.text
        shortcut: qaction.shortcutName()
        icon.name: qaction.iconName()

        Component.onCompleted: list["frameStepAction"] = frameStepAction

        onTriggered: mpv.command(["frame-step"])
    }

    property Action frameBackStepAction: Action {
        id: frameBackStepAction
        property var qaction: app.action("frameBackStep")
        text: qaction.text
        shortcut: qaction.shortcutName()
        icon.name: qaction.iconName()

        Component.onCompleted: list["frameBackStepAction"] = frameBackStepAction

        onTriggered: mpv.command(["frame-back-step"])
    }

    property Action increasePlayBackSpeedAction: Action {
        id: increasePlayBackSpeedAction
        property var qaction: app.action("increasePlayBackSpeed")
        text: qaction.text
        shortcut: qaction.shortcutName()
        icon.name: qaction.iconName()

        Component.onCompleted: list["increasePlayBackSpeedAction"] = increasePlayBackSpeedAction

        onTriggered: {
            mpv.command(["add", "speed", "0.1"])
            osd.message(`Speed: ${mpv.getProperty("speed").toFixed(2)}`)
        }
    }

    property Action decreasePlayBackSpeedAction: Action {
        id: decreasePlayBackSpeedAction
        property var qaction: app.action("decreasePlayBackSpeed")
        text: qaction.text
        shortcut: qaction.shortcutName()
        icon.name: qaction.iconName()

        Component.onCompleted: list["decreasePlayBackSpeedAction"] = decreasePlayBackSpeedAction

        onTriggered: {
            mpv.command(["add", "speed", "-0.1"])
            osd.message(`Speed: ${mpv.getProperty("speed").toFixed(2)}`)
        }
    }

    property Action resetPlayBackSpeedAction: Action {
        id: resetPlayBackSpeedAction
        property var qaction: app.action("resetPlayBackSpeed")
        text: qaction.text
        shortcut: qaction.shortcutName()
        icon.name: qaction.iconName()

        Component.onCompleted: list["resetPlayBackSpeedAction"] = resetPlayBackSpeedAction

        onTriggered: {
            mpv.setProperty("speed", 1.0)
            osd.message(`Speed: ${mpv.getProperty("speed").toFixed(2)}`)
         }
    }

    property Action playPauseAction: Action {
        id: playPauseAction
        text: qsTr("Play/Pause")
        icon.name: "media-playback-pause"
        shortcut: "Space"

        Component.onCompleted: list["playPauseAction"] = playPauseAction

        onTriggered: mpv.setProperty("pause", !mpv.getProperty("pause"))
    }

    property Action configureShortcutsAction: Action {
        id: configureShortcutsAction
        property var qaction: app.action("options_configure_keybinding")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["configureShortcutsAction"] = configureShortcutsAction

        onTriggered: qaction.trigger()
    }

    property Action quitApplicationAction: Action {
        id: quitApplicationAction
        property var qaction: app.action("file_quit")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["quitApplicationAction"] = quitApplicationAction

        onTriggered: {
            mpv.handleTimePosition()
            qaction.trigger()
        }
    }

    property Action configureAction: Action {
        id: configureAction
        property var qaction: app.action("configure")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["configureAction"] = configureAction

        onTriggered: {
            settingsEditor.visible = true
        }
    }

    property Action subtitleQuickenAction: Action {
        id: subtitleQuickenAction
        property var qaction: app.action("subtitleQuicken")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["subtitleQuickenAction"] = subtitleQuickenAction

        onTriggered: {
            mpv.setProperty("sub-delay", mpv.getProperty("sub-delay") - 0.1)
            osd.message(`Subtitle timing: ${mpv.getProperty("sub-delay").toFixed(2)}`)
        }
    }

    property Action subtitleDelayAction: Action {
        id: subtitleDelayAction
        property var qaction: app.action("subtitleDelay")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["subtitleDelayAction"] = subtitleDelayAction

        onTriggered: {
            mpv.setProperty("sub-delay", mpv.getProperty("sub-delay") + 0.1)
            osd.message(`Subtitle timing: ${mpv.getProperty("sub-delay").toFixed(2)}`)
        }
    }

    property Action subtitleToggleAction: Action {
        id: subtitleToggleAction
        property var qaction: app.action("subtitleToggle")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["subtitleToggleAction"] = subtitleToggleAction

        onTriggered: {
            const visible = mpv.getProperty("sub-visibility")
            const message = visible ? "Subtitles off" : "Subtitles on"
            mpv.setProperty("sub-visibility", !visible)
            osd.message(message)
        }
    }

    property Action audioCycleUpAction: Action {
        id: audioCycleUpAction
        property var qaction: app.action("audioCycleUp")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["audioCycleUpAction"] = audioCycleUpAction

        onTriggered: {
            const tracks = mpv.getProperty("track-list")
            let audioTracksCount = 0
            tracks.forEach(t => { if(t.type === "audio") ++audioTracksCount })

            if (audioTracksCount > 1) {
                mpv.command(["cycle", "aid", "up"])
                const currentTrackId = mpv.getProperty("aid")

                if (currentTrackId === false) {
                    audioCycleUpAction.trigger()
                    return
                }
                const track = tracks.find(t => t.type === "audio" && t.id === currentTrackId)
                const message = `Audio: ${currentTrackId} (${track.lang})`
                osd.message(message)
            }
        }
    }

    property Action audioCycleDownAction: Action {
        id: audioCycleDownAction
        property var qaction: app.action("audioCycleDown")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["audioCycleDownAction"] = audioCycleDownAction

        onTriggered: {
            const tracks = mpv.getProperty("track-list")
            let audioTracksCount = 0
            tracks.forEach(t => { if(t.type === "audio") ++audioTracksCount })

            if (audioTracksCount > 1) {
                mpv.command(["cycle", "aid", "down"])
                const currentTrackId = mpv.getProperty("aid")

                if (currentTrackId === false) {
                    audioCycleDownAction.trigger()
                    return
                }
                const track = tracks.find(t => t.type === "audio" && t.id === currentTrackId)
                const message = `Audio: ${currentTrackId} (${track.lang})`
                osd.message(message)
            }
        }
    }

    property Action subtitleCycleUpAction: Action {
        id: subtitleCycleUpAction
        property var qaction: app.action("subtitleCycleUp")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["subtitleCycleUpAction"] = subtitleCycleUpAction

        onTriggered: {
            mpv.command(["cycle", "sid", "up"])
            const currentTrackId = mpv.getProperty("sid")
            let message;
            if (currentTrackId === false) {
                message = `Subtitle: None`
            } else {
                const tracks = mpv.getProperty("track-list")
                const track = tracks.find(t => t.type === "sub" && t.id === currentTrackId)
                message = `Subtitle: ${currentTrackId} (${track.lang})`
            }
            osd.message(message)
        }
    }

    property Action subtitleCycleDownAction: Action {
        id: subtitleCycleDownAction
        property var qaction: app.action("subtitleCycleDown")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["subtitleCycleDownAction"] = subtitleCycleDownAction

        onTriggered: {
            mpv.command(["cycle", "sid", "down"])
            const currentTrackId = mpv.getProperty("sid")
            let message;
            if (currentTrackId === false) {
                message = `Subtitle: None`
            } else {
                const tracks = mpv.getProperty("track-list")
                const track = tracks.find(t => t.type === "sub" && t.id === currentTrackId)
                message = `Subtitle: ${currentTrackId} (${track.lang})`
            }
            osd.message(message)
        }
    }

    property Action contrastUpAction: Action {
        id: contrastUpAction
        property var qaction: app.action("contrastUp")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["contrastUpAction"] = contrastUpAction

        onTriggered: {
            const contrast = parseInt(mpv.getProperty("contrast")) + 1
            mpv.setProperty("contrast", `${contrast}`)
            osd.message(`Contrast: ${contrast}`)
        }
    }
    property Action contrastDownAction: Action {
        id: contrastDownAction
        property var qaction: app.action("contrastDown")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["contrastDownAction"] = contrastDownAction

        onTriggered: {
            const contrast = parseInt(mpv.getProperty("contrast")) - 1
            mpv.setProperty("contrast", `${contrast}`)
            osd.message(`Contrast: ${contrast}`)
        }
    }
    property Action contrastResetAction: Action {
        id: contrastResetAction
        property var qaction: app.action("contrastReset")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["contrastResetAction"] = contrastResetAction

        onTriggered: {
            mpv.setProperty("contrast", `0`)
            osd.message(`Contrast: 0`)
        }
    }

    property Action brightnessUpAction: Action {
        id: brightnessUpAction
        property var qaction: app.action("brightnessUp")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["brightnessUpAction"] = brightnessUpAction

        onTriggered: {
            const brightness = parseInt(mpv.getProperty("brightness")) + 1
            mpv.setProperty("brightness", `${brightness}`)
            osd.message(`Brightness: ${brightness}`)
        }
    }
    property Action brightnessDownAction: Action {
        id: brightnessDownAction
        property var qaction: app.action("brightnessDown")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["brightnessDownAction"] = brightnessDownAction

        onTriggered: {
            const brightness = parseInt(mpv.getProperty("brightness")) - 1
            mpv.setProperty("brightness", `${brightness}`)
            osd.message(`Brightness: ${brightness}`)
        }
    }
    property Action brightnessResetAction: Action {
        id: brightnessResetAction
        property var qaction: app.action("brightnessReset")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["brightnessResetAction"] = brightnessResetAction

        onTriggered: {
            mpv.setProperty("brightness", `0`)
            osd.message(`Brightness: 0`)
        }
    }
    property Action gammaUpAction: Action {
        id: gammaUpAction
        property var qaction: app.action("gammaUp")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["gammaUpAction"] = gammaUpAction

        onTriggered: {
            const gamma = parseInt(mpv.getProperty("gamma")) + 1
            mpv.setProperty("gamma", `${gamma}`)
            osd.message(`Gamma: ${gamma}`)
        }
    }
    property Action gammaDownAction: Action {
        id: gammaDownAction
        property var qaction: app.action("gammaDown")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["gammaDownAction"] = gammaDownAction

        onTriggered: {
            const gamma = parseInt(mpv.getProperty("gamma")) - 1
            mpv.setProperty("gamma", `${gamma}`)
            osd.message(`Gamma: ${gamma}`)
        }
    }
    property Action gammaResetAction: Action {
        id: gammaResetAction
        property var qaction: app.action("gammaReset")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["gammaResetAction"] = gammaResetAction

        onTriggered: {
            mpv.setProperty("gamma", `0`)
            osd.message(`Gamma: 0`)
        }
    }
    property Action saturationUpAction: Action {
        id: saturationUpAction
        property var qaction: app.action("saturationUp")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["saturationUpAction"] = saturationUpAction

        onTriggered: {
            const saturation = parseInt(mpv.getProperty("saturation")) + 1
            mpv.setProperty("saturation", `${saturation}`)
            osd.message(`Saturation: ${saturation}`)
        }
    }
    property Action saturationDownAction: Action {
        id: saturationDownAction
        property var qaction: app.action("saturationDown")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["saturationDownAction"] = saturationDownAction

        onTriggered: {
            const saturation = parseInt(mpv.getProperty("saturation")) - 1
            mpv.setProperty("saturation", `${saturation}`)
            osd.message(`Saturation: ${saturation}`)
        }
    }
    property Action saturationResetAction: Action {
        id: saturationResetAction
        property var qaction: app.action("saturationReset")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["saturationResetAction"] = saturationResetAction

        onTriggered: {
            mpv.setProperty("saturation", `0`)
            osd.message(`Saturation: 0`)
        }
    }

    property Action zoomInAction: Action {
        id: zoomInAction
        property var qaction: app.action("zoomIn")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["zoomInAction"] = zoomInAction

        onTriggered: {
            const zoom = mpv.getProperty("video-zoom") + 0.1
            mpv.setProperty("video-zoom", zoom)
            osd.message(`Zoom: ${zoom.toFixed(2)}`)
        }
    }

    property Action zoomOutAction: Action {
        id: zoomOutAction
        property var qaction: app.action("zoomOut")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["zoomOutAction"] = zoomOutAction

        onTriggered: {
            const zoom = mpv.getProperty("video-zoom") - 0.1
            mpv.setProperty("video-zoom", zoom)
            osd.message(`Zoom: ${zoom.toFixed(2)}`)
        }
    }
    property Action zoomResetAction: Action {
        id: zoomResetAction
        property var qaction: app.action("zoomReset")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["zoomResetAction"] = zoomResetAction

        onTriggered: {
            mpv.setProperty("video-zoom", 0)
            osd.message(`Zoom: 0`)
        }
    }


    property Action videoPanXLeftAction: Action {
        id: videoPanXLeftAction
        property var qaction: app.action("videoPanXLeft")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["videoPanXLeftAction"] = videoPanXLeftAction

        onTriggered: {
            const pan = mpv.getProperty("video-pan-x") - 0.01
            mpv.setProperty("video-pan-x", pan)
            osd.message(`Video pan x: ${pan.toFixed(2)}`)
        }
    }
    property Action videoPanXRightAction: Action {
        id: videoPanXRightAction
        property var qaction: app.action("videoPanXRight")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["videoPanXRightAction"] = videoPanXRightAction

        onTriggered: {
            const pan = mpv.getProperty("video-pan-x") + 0.01
            mpv.setProperty("video-pan-x", pan)
            osd.message(`Video pan x: ${pan.toFixed(2)}`)
        }
    }
    property Action videoPanYUpAction: Action {
        id: videoPanYUpAction
        property var qaction: app.action("videoPanYUp")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["videoPanYUpAction"] = videoPanYUpAction

        onTriggered: {
            const pan = mpv.getProperty("video-pan-y") - 0.01
            mpv.setProperty("video-pan-y", pan)
            osd.message(`Video pan x: ${pan.toFixed(2)}`)
        }
    }
    property Action videoPanYDownAction: Action {
        id: videoPanYDownAction
        property var qaction: app.action("videoPanYDown")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["videoPanYDownAction"] = videoPanYDownAction

        onTriggered: {
            const pan = mpv.getProperty("video-pan-y") + 0.01
            mpv.setProperty("video-pan-y", pan)
            osd.message(`Video pan x: ${pan.toFixed(2)}`)
        }
    }

    property Action toggleFullscreenAction: Action {
        id: toggleFullscreenAction
        property var qaction: app.action("toggleFullscreen")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["toggleFullscreenAction"] = toggleFullscreenAction

        onTriggered: {
            window.toggleFullScreen()
        }
    }

    property Action toggleMenuBarAction: Action {
        id: toggleMenuBarAction
        property var qaction: app.action("toggleMenuBar")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["toggleMenuBarAction"] = toggleMenuBarAction

        onTriggered: GeneralSettings.showMenuBar = !menuBar.visible
    }

    property Action toggleHeaderAction: Action {
        id: toggleHeaderAction
        property var qaction: app.action("toggleHeader")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["toggleHeaderAction"] = toggleHeaderAction

        onTriggered: GeneralSettings.showHeader = !header.visible
    }

    property Action screenshotAction: Action {
        id: screenshotAction
        property var qaction: app.action("screenshot")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["screenshotAction"] = screenshotAction

        onTriggered: mpv.command(["screenshot"])
    }

    property Action setLoopAction: Action {
        id: setLoopAction
        property var qaction: app.action("setLoop")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["setLoopAction"] = setLoopAction

        onTriggered: {
            var a = mpv.getProperty("ab-loop-a")
            var b = mpv.getProperty("ab-loop-b")

            var aIsSet = a !== "no"
            var bIsSet = b !== "no"

            if (!aIsSet && !bIsSet) {
                mpv.setProperty("ab-loop-a", mpv.position)
                footer.progressBar.loopIndicator.startPosition = mpv.position
                osd.message("Loop start: " + app.formatTime(mpv.position))
            } else if (aIsSet && !bIsSet) {
                mpv.setProperty("ab-loop-b", mpv.position)
                footer.progressBar.loopIndicator.endPosition = mpv.position
                osd.message(`Loop: ${app.formatTime(a)} - ${app.formatTime(mpv.position)}`)
            } else if (aIsSet && bIsSet) {
                mpv.setProperty("ab-loop-a", "no")
                mpv.setProperty("ab-loop-b", "no")
                footer.progressBar.loopIndicator.startPosition = -1
                footer.progressBar.loopIndicator.endPosition = -1
                osd.message("Loop cleared")
            }
        }
    }

    property Action increaseSubtitleFontSizeAction: Action {
        id: increaseSubtitleFontSizeAction
        property var qaction: app.action("increaseSubtitleFontSize")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["increaseSubtitleFontSizeAction"] = increaseSubtitleFontSizeAction

        onTriggered: {
            mpv.command(["add", "sub-scale", "+0.1"])
            osd.message(qsTr("Subtitle scale: " + mpv.getProperty("sub-scale").toFixed(1)))
        }
    }

    property Action decreaseSubtitleFontSizeAction: Action {
        id: decreaseSubtitleFontSizeAction
        property var qaction: app.action("decreaseSubtitleFontSize")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["decreaseSubtitleFontSizeAction"] = decreaseSubtitleFontSizeAction

        onTriggered: {
            mpv.command(["add", "sub-scale", "-0.1"])
            osd.message(qsTr("Subtitle scale: " + mpv.getProperty("sub-scale").toFixed(1)))
        }
    }

    property Action subtitlePositionUpAction: Action {
        id: subtitlePositionUpAction
        property var qaction: app.action("subtitlePositionUp")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["subtitlePositionUpAction"] = subtitlePositionUpAction

        onTriggered: {
            mpv.command(["add", "sub-pos", "-1"])
        }
    }

    property Action subtitlePositionDownAction: Action {
        id: subtitlePositionDownAction
        property var qaction: app.action("subtitlePositionDown")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["subtitlePositionDownAction"] = subtitlePositionDownAction

        onTriggered: {
            mpv.command(["add", "sub-pos", "+1"])
        }
    }

    property Action toggleDeinterlacingAction: Action {
        id: toggleDeinterlacingAction
        property var qaction: app.action("toggleDeinterlacing")
        text: qaction.text
        icon.name: qaction.iconName()
        shortcut: qaction.shortcutName()

        Component.onCompleted: list["toggleDeinterlacingAction"] = toggleDeinterlacingAction

        onTriggered: {
            mpv.setProperty("deinterlace", !mpv.getProperty("deinterlace"))
            osd.message(`Deinterlace: ${mpv.getProperty("deinterlace")}`)
        }
    }
}
