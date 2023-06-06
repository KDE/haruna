/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

import org.kde.haruna 1.0
import "Settings"

Item {
    id: root

    Instantiator {
        model: actionsModel
        delegate: Action {
            objectName: model.name
            text: model.text
            shortcut: model.shortcut
            icon.name: model.icon
            onTriggered: {
                if (model.type === "NormalAction") {
                    actionsModel.signalEmitter(objectName)
                }
                if (model.type === "CustomAction") {
                    mpv.userCommand(model.text)
                    osd.message(mpv.expandText(model.description))
                }
            }

            Component.onCompleted: appActions[objectName] = this
        }
    }

    Connections {
        target: actionsModel

        onAboutHarunaAction: {
            settingsLoader.active = true
            settingsLoader.item.currentPage = SettingsWindow.Page.About
            actionsModel.signalEmitter("configureAction")
        }

        onAudioCycleUpAction: {
            // TODO: fix osd showing wrong track caused by mpv running in a separate thread
            const tracks = mpv.getProperty("track-list")
            let audioTracksCount = 0
            tracks.forEach(t => { if(t.type === "audio") ++audioTracksCount })

            if (audioTracksCount > 1) {
                mpv.command(["cycle", "aid", "up"])
                const currentTrackId = mpv.getProperty("aid")

                if (currentTrackId === false) {
                    actionsModel.signalEmitter("audioCycleUpAction")
                    return
                }
                const track = tracks.find(t => t.type === "audio" && t.id === currentTrackId)
                osd.message(i18nc("@info:tooltip", "Audio: %1 %2", currentTrackId, track.lang))
            }
        }

        onAudioCycleDownAction: {
            // TODO: fix osd showing wrong track caused by mpv running in a separate thread
            const tracks = mpv.getProperty("track-list")
            let audioTracksCount = 0
            tracks.forEach(t => { if(t.type === "audio") ++audioTracksCount })

            if (audioTracksCount > 1) {
                mpv.command(["cycle", "aid", "down"])
                const currentTrackId = mpv.getProperty("aid")

                if (currentTrackId === false) {
                    actionsModel.signalEmitter("audioCycleDownAction")
                    return
                }
                const track = tracks.find(t => t.type === "audio" && t.id === currentTrackId)
                osd.message(i18nc("@info:tooltip", "Audio: %1 %2", currentTrackId, track.lang))
            }
        }

        onConfigureAction: {
            settingsLoader.active = true
            if (settingsLoader.item.visible) {
                settingsLoader.item.raise()
            } else {
                settingsLoader.item.visible = true
            }
        }

        onConfigureShortcutsAction: {
            settingsLoader.active = true
            settingsLoader.item.currentPage = SettingsWindow.Page.Shortcuts
            actionsModel.signalEmitter("configureAction")
        }

        onExitFullscreenAction: {
            window.exitFullscreen()
        }

        onFrameStepForwardAction: mpv.command(["frame-step"])

        onFrameStepBackwardAction: mpv.command(["frame-back-step"])

        onLoadLastPlayedFileAction: window.openFile(GeneralSettings.lastPlayedFile, true)

        onMuteAction: mpv.mute = !mpv.mute

        onOpenContextMenuAction: {
            mpvContextMenuLoader.active = true
            mpvContextMenuLoader.item.popup()
        }

        onOpenFileAction: fileDialog.open()

        onOpenSubtitlesFileAction: subtitlesFileDialog.open()

        onOpenUrlAction: {
            if (openUrlPopup.visible) {
                openUrlPopup.close()
            } else {
                openUrlPopup.open()
            }
        }

        onPlaybackSpeedIncreaseAction: {
            const speed = mpv.getProperty("speed") + 0.1
            mpv.setProperty("speed", speed)
            osd.message(i18nc("@info:tooltip", "Playback speed: %1", speed.toFixed(2)))
        }

        onPlaybackSpeedDecreaseAction: {
            const speed = mpv.getProperty("speed") - 0.1
            mpv.setProperty("speed", speed)
            osd.message(i18nc("@info:tooltip", "Playback speed: %1", speed.toFixed(2)))
        }

        onPlaybackSpeedResetAction: {
            mpv.setProperty("speed", 1.0)
            osd.message(i18nc("@info:tooltip", "Playback speed: %1", 1.0))
        }

        onPlayPauseAction: mpv.pause = !mpv.pause

        onPlayNextAction: {
            mpv.playlistProxyModel.playNext()
        }

        onPlayPreviousAction: {
            mpv.playlistProxyModel.playPrevious()
        }

        onQuitApplicationAction: {
            Qt.quit()
        }

        onRestartPlaybackAction: mpv.command(["seek", 0, "absolute"])

        onSeekForwardSmallAction: mpv.command(["seek", GeneralSettings.seekSmallStep, "exact"])

        onSeekBackwardSmallAction: mpv.command(["seek", -GeneralSettings.seekSmallStep, "exact"])

        onSeekForwardMediumAction: mpv.command(["seek", GeneralSettings.seekMediumStep, "exact"])

        onSeekBackwardMediumAction: mpv.command(["seek", -GeneralSettings.seekMediumStep, "exact"])

        onSeekForwardBigAction: mpv.command(["seek", GeneralSettings.seekBigStep, "exact"])

        onSeekBackwardBigAction: mpv.command(["seek", -GeneralSettings.seekBigStep, "exact"])

        onSeekNextChapterAction: {
            const chapters = mpv.getProperty("chapter-list")
            const currentChapter = mpv.getProperty("chapter")
            const nextChapter = currentChapter + 1
            if (nextChapter === chapters.length) {
                actionsModel.signalEmitter("playNextAction")
                return
            }
            mpv.command(["add", "chapter", "1"])
        }

        onSeekPreviousChapterAction: mpv.command(["add", "chapter", "-1"])

        onSeekNextSubtitleAction: {
            if (mpv.getProperty("sid") !== false) {
                mpv.command(["sub-seek", "1"])
            } else {
                actionsModel.signalEmitter("seekForwardSmallAction")
            }
        }

        onSeekPreviousSubtitleAction: {
            if (mpv.getProperty("sid") !== false) {
                mpv.command(["sub-seek", "-1"])
            } else {
                actionsModel.signalEmitter("seekBackwardSmallAction")
            }
        }

        onSeekToWatchLaterPositionAction: {
            if (mpv.watchLaterPosition === 0) {
                return
            }
            mpv.command(["seek", mpv.watchLaterPosition, "absolute"])
        }

        onSetLoopAction: {
            var a = mpv.getProperty("ab-loop-a")
            var b = mpv.getProperty("ab-loop-b")

            var aIsSet = a !== "no"
            var bIsSet = b !== "no"

            if (!aIsSet && !bIsSet) {
                mpv.setProperty("ab-loop-a", mpv.position)
                footer.progressBar.loopIndicator.startPosition = mpv.position
                osd.message(i18nc("@info:tooltip", "Loop start: %1", app.formatTime(mpv.position)))
            } else if (aIsSet && !bIsSet) {
                mpv.setProperty("ab-loop-b", mpv.position)
                footer.progressBar.loopIndicator.endPosition = mpv.position
                osd.message(i18nc("@info:tooltip", "Loop: %1 - %2", app.formatTime(a), app.formatTime(mpv.position)))
            } else if (aIsSet && bIsSet) {
                mpv.setProperty("ab-loop-a", "no")
                mpv.setProperty("ab-loop-b", "no")
                footer.progressBar.loopIndicator.startPosition = -1
                footer.progressBar.loopIndicator.endPosition = -1
                osd.message(i18nc("@info:tooltip", "Loop cleared"))
            }
        }

        onScreenshotAction: mpv.command(["screenshot"])

        onSubtitleQuickenAction: {
            const delay = mpv.getProperty("sub-delay") - 0.1
            mpv.setProperty("sub-delay", delay)
            osd.message(i18nc("@info:tooltip", "Subtitle timing: %1", delay.toFixed(2)))
        }

        onSubtitleDelayAction: {
            const delay = mpv.getProperty("sub-delay") + 0.1
            mpv.setProperty("sub-delay", delay)
            osd.message(i18nc("@info:tooltip", "Subtitle timing: %1", delay.toFixed(2)))
        }

        onSubtitleToggleAction: {
            const visible = mpv.getProperty("sub-visibility")
            const message = visible ? i18nc("@info:tooltip", "Subtitles off") : i18nc("@info:tooltip", "Subtitles on")
            mpv.setProperty("sub-visibility", !visible)
            osd.message(message)
        }

        onSubtitleCycleUpAction: {
            // TODO: fix osd showing wrong track caused by mpv running in a separate thread
            mpv.command(["cycle", "sid", "up"])
            const currentTrackId = mpv.getProperty("sid")
            if (currentTrackId === false) {
                osd.message(i18nc("@info:tooltip", "Subtitle: None"))
            } else {
                const tracks = mpv.getProperty("track-list")
                const track = tracks.find(t => t.type === "sub" && t.id === currentTrackId)
                osd.message(i18nc("@info:tooltip", "Subtitle: %1 %2", currentTrackId, track.lang))
            }
        }

        onSubtitleCycleDownAction: {
            // TODO: fix osd showing wrong track caused by mpv running in a separate thread
            mpv.command(["cycle", "sid", "down"])
            const currentTrackId = mpv.getProperty("sid")
            if (currentTrackId === false) {
                osd.message(i18nc("@info:tooltip", "Subtitle: None"))
            } else {
                const tracks = mpv.getProperty("track-list")
                const track = tracks.find(t => t.type === "sub" && t.id === currentTrackId)
                osd.message(i18nc("@info:tooltip", "Subtitle: %1 %2", currentTrackId, track.lang))
            }
        }

        onSubtitleIncreaseFontSizeAction: {
            const subScale = mpv.getProperty("sub-scale") + 0.1
            mpv.setProperty("sub-scale", subScale)
            osd.message(i18nc("@info:tooltip", "Subtitle scale: %1", subScale.toFixed(1)))
        }

        onSubtitleDecreaseFontSizeAction: {
            const subScale = mpv.getProperty("sub-scale") - 0.1
            mpv.setProperty("sub-scale", subScale)
            osd.message(i18nc("@info:tooltip", "Subtitle scale: %1", subScale.toFixed(1)))
        }

        onSubtitleMoveUpAction: mpv.command(["add", "sub-pos", "-1"])

        onSubtitleMoveDownAction: mpv.command(["add", "sub-pos", "+1"])

        onToggleDeinterlacingAction: {
            const deinterlaced = !mpv.getProperty("deinterlace")
            mpv.setProperty("deinterlace", deinterlaced)
            osd.message(i18nc("@info:tooltip", "Deinterlace: %1", deinterlaced))
        }

        onToggleFullscreenAction: window.toggleFullScreen()

        onToggleMenuBarAction: {
            GeneralSettings.showMenuBar = !GeneralSettings.showMenuBar
            GeneralSettings.save()
        }

        onToggleHeaderAction: {
            GeneralSettings.showHeader = !GeneralSettings.showHeader
            GeneralSettings.save()
        }

        onTogglePlaylistAction: {
            if (playList.state === "visible") {
                playList.state = "hidden"
            } else {
                playList.state = "visible"
            }
        }

        onVideoPanXLeftAction: {
            const pan = mpv.getProperty("video-pan-x") - 0.01
            mpv.setProperty("video-pan-x", pan)
        }

        onVideoPanXRightAction: {
            const pan = mpv.getProperty("video-pan-x") + 0.01
            mpv.setProperty("video-pan-x", pan)
        }

        onVideoPanYUpAction: {
            const pan = mpv.getProperty("video-pan-y") - 0.01
            mpv.setProperty("video-pan-y", pan)
        }

        onVideoPanYDownAction: {
            const pan = mpv.getProperty("video-pan-y") + 0.01
            mpv.setProperty("video-pan-y", pan)
        }

        onVolumeUpAction: {
            mpv.command(["add", "volume", GeneralSettings.volumeStep])
        }

        onVolumeDownAction: {
            mpv.command(["add", "volume", -GeneralSettings.volumeStep])
        }

        onZoomInAction: {
            const zoom = mpv.getProperty("video-zoom") + 0.1
            mpv.setProperty("video-zoom", zoom)
            osd.message(i18nc("@info:tooltip", "Zoom: %1", zoom.toFixed(2)))
        }

        onZoomOutAction: {
            const zoom = mpv.getProperty("video-zoom") - 0.1
            mpv.setProperty("video-zoom", zoom)
            osd.message(i18nc("@info:tooltip", "Zoom: %1", zoom.toFixed(2)))
        }

        onZoomResetAction: {
            mpv.setProperty("video-zoom", 0)
            osd.message(i18nc("@info:tooltip", "Zoom: 0"))
        }


        onContrastUpAction: {
            const contrast = parseInt(mpv.getProperty("contrast")) + 1
            mpv.setProperty("contrast", `${contrast}`)
            osd.message(i18nc("@info:tooltip", "Contrast: %1", contrast))
        }

        onContrastDownAction: {
            const contrast = parseInt(mpv.getProperty("contrast")) - 1
            mpv.setProperty("contrast", `${contrast}`)
            osd.message(i18nc("@info:tooltip", "Contrast: %1", contrast))
        }

        onContrastResetAction: {
            mpv.setProperty("contrast", 0)
            osd.message(i18nc("@info:tooltip", "Contrast: 0"))
        }

        onBrightnessUpAction: {
            const brightness = parseInt(mpv.getProperty("brightness")) + 1
            mpv.setProperty("brightness", `${brightness}`)
            osd.message(i18nc("@info:tooltip", "Brightness: %1", brightness))
        }

        onBrightnessDownAction: {
            const brightness = parseInt(mpv.getProperty("brightness")) - 1
            mpv.setProperty("brightness", `${brightness}`)
            osd.message(i18nc("@info:tooltip", "Brightness: %1", brightness))
        }

        onBrightnessResetAction: {
            mpv.setProperty("brightness", 0)
            osd.message(i18nc("@info:tooltip", "Brightness: 0"))
        }

        onGammaUpAction: {
            const gamma = parseInt(mpv.getProperty("gamma")) + 1
            mpv.setProperty("gamma", `${gamma}`)
            osd.message(i18nc("@info:tooltip", "Gamma: %1", gamma))
        }

        onGammaDownAction: {
            const gamma = parseInt(mpv.getProperty("gamma")) - 1
            mpv.setProperty("gamma", `${gamma}`)
            osd.message(i18nc("@info:tooltip", "Gamma: %1", gamma))
        }

        onGammaResetAction: {
            mpv.setProperty("gamma", 0)
            osd.message(i18nc("@info:tooltip", "Gamma: 0"))
        }

        onSaturationUpAction: {
            const saturation = parseInt(mpv.getProperty("saturation")) + 1
            mpv.setProperty("saturation", `${saturation}`)
            osd.message(i18nc("@info:tooltip", "Saturation: %1", saturation))
        }

        onSaturationDownAction: {
            const saturation = parseInt(mpv.getProperty("saturation")) - 1
            mpv.setProperty("saturation", `${saturation}`)
            osd.message(i18nc("@info:tooltip", "Saturation: %1", saturation))
        }

        onSaturationResetAction: {
            mpv.setProperty("saturation", 0)
            osd.message(i18nc("@info:tooltip", "Saturation: 0"))
        }
    }
}
