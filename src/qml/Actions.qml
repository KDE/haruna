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

        onOpenActionsDialogAction : {
            triggerActionPopup.open()
        }

        onAboutHarunaAction: {
            settingsLoader.active = true
            settingsLoader.item.currentPage = SettingsWindow.Page.About
            actionsModel.signalEmitter("configureAction")
        }

        onAudioCycleUpAction: {
            const tracks = mpv.getProperty(MpvProperties.TrackList)
            let audioTracksCount = 0
            tracks.forEach(t => { if(t.type === "audio") ++audioTracksCount })

            if (audioTracksCount > 1) {
                mpv.synchronousCommand(["cycle", "aid", "up"])
                const currentTrackId = mpv.getProperty(MpvProperties.AudioId)

                if (currentTrackId === false) {
                    actionsModel.signalEmitter("audioCycleUpAction")
                    return
                }
                const track = tracks.find(t => t.type === "audio" && t.id === currentTrackId)
                osd.message(i18nc("@info:tooltip", "Audio: %1 %2", currentTrackId, track.lang || ""))
            }
        }

        onAudioCycleDownAction: {
            const tracks = mpv.getProperty(MpvProperties.TrackList)
            let audioTracksCount = 0
            tracks.forEach(t => { if(t.type === "audio") ++audioTracksCount })

            if (audioTracksCount > 1) {
                mpv.synchronousCommand(["cycle", "aid", "down"])
                const currentTrackId = mpv.getProperty(MpvProperties.AudioId)

                if (currentTrackId === false) {
                    actionsModel.signalEmitter("audioCycleDownAction")
                    return
                }
                const track = tracks.find(t => t.type === "audio" && t.id === currentTrackId)
                osd.message(i18nc("@info:tooltip", "Audio: %1 %2", currentTrackId, track.lang || ""))
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

        onFrameStepForwardAction: mpv.commandAsync(["frame-step"])

        onFrameStepBackwardAction: mpv.commandAsync(["frame-back-step"])

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
            const speed = mpv.getProperty(MpvProperties.Speed) + 0.1
            mpv.setProperty(MpvProperties.Speed, speed)
            osd.message(i18nc("@info:tooltip", "Playback speed: %1", speed.toFixed(2)))
        }

        onPlaybackSpeedDecreaseAction: {
            const speed = mpv.getProperty(MpvProperties.Speed) - 0.1
            mpv.setProperty(MpvProperties.Speed, speed)
            osd.message(i18nc("@info:tooltip", "Playback speed: %1", speed.toFixed(2)))
        }

        onPlaybackSpeedResetAction: {
            mpv.setProperty(MpvProperties.Speed, 1.0)
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

        onRestartPlaybackAction: mpv.commandAsync(["seek", 0, "absolute"])

        onSeekForwardSmallAction: mpv.commandAsync(["seek", GeneralSettings.seekSmallStep, "exact"])

        onSeekBackwardSmallAction: mpv.commandAsync(["seek", -GeneralSettings.seekSmallStep, "exact"])

        onSeekForwardMediumAction: mpv.commandAsync(["seek", GeneralSettings.seekMediumStep, "exact"])

        onSeekBackwardMediumAction: mpv.commandAsync(["seek", -GeneralSettings.seekMediumStep, "exact"])

        onSeekForwardBigAction: mpv.commandAsync(["seek", GeneralSettings.seekBigStep, "exact"])

        onSeekBackwardBigAction: mpv.commandAsync(["seek", -GeneralSettings.seekBigStep, "exact"])

        onSeekNextChapterAction: {
            const chapters = mpv.getProperty(MpvProperties.ChapterList)
            const currentChapter = mpv.getProperty(MpvProperties.Chapter)
            const nextChapter = currentChapter + 1
            if (nextChapter === chapters.length) {
                actionsModel.signalEmitter("playNextAction")
                return
            }
            mpv.commandAsync(["add", "chapter", "1"])
        }

        onSeekPreviousChapterAction: mpv.commandAsync(["add", "chapter", "-1"])

        onSeekNextSubtitleAction: {
            if (mpv.getProperty(MpvProperties.SubtitleId) !== false) {
                mpv.commandAsync(["sub-seek", "1"])
            } else {
                actionsModel.signalEmitter("seekForwardSmallAction")
            }
        }

        onSeekPreviousSubtitleAction: {
            if (mpv.getProperty(MpvProperties.SubtitleId) !== false) {
                mpv.commandAsync(["sub-seek", "-1"])
            } else {
                actionsModel.signalEmitter("seekBackwardSmallAction")
            }
        }

        onSeekToWatchLaterPositionAction: {
            if (mpv.watchLaterPosition === 0) {
                return
            }
            mpv.commandAsync(["seek", mpv.watchLaterPosition, "absolute"])
        }

        onSetLoopAction: {
            var a = mpv.getProperty(MpvProperties.ABLoopA)
            var b = mpv.getProperty(MpvProperties.ABLoopB)

            var aIsSet = a !== "no"
            var bIsSet = b !== "no"

            if (!aIsSet && !bIsSet) {
                mpv.setProperty(MpvProperties.ABLoopA, mpv.position)
                footer.progressBar.loopIndicator.startPosition = mpv.position
                osd.message(i18nc("@info:tooltip", "Loop start: %1", app.formatTime(mpv.position)))
            } else if (aIsSet && !bIsSet) {
                mpv.setProperty(MpvProperties.ABLoopB, mpv.position)
                footer.progressBar.loopIndicator.endPosition = mpv.position
                osd.message(i18nc("@info:tooltip", "Loop: %1 - %2", app.formatTime(a), app.formatTime(mpv.position)))
            } else if (aIsSet && bIsSet) {
                mpv.setProperty(ABLoopA, "no")
                mpv.setProperty(ABLoopB, "no")
                footer.progressBar.loopIndicator.startPosition = -1
                footer.progressBar.loopIndicator.endPosition = -1
                osd.message(i18nc("@info:tooltip", "Loop cleared"))
            }
        }

        onScreenshotAction: {
            const ss = mpv.synchronousCommand(["screenshot"])
            if (ss.filename) {
                osd.message(i18nc("@info:tooltip", "Screenshot: %1", ss.filename))
            }
        }

        onSubtitleQuickenAction: {
            const delay = mpv.getProperty(MpvProperties.SubtitleDelay) - 0.1
            mpv.setProperty(MpvProperties.SubtitleDelay, delay)
            osd.message(i18nc("@info:tooltip", "Subtitle timing: %1", delay.toFixed(2)))
        }

        onSubtitleDelayAction: {
            const delay = mpv.getProperty(MpvProperties.SubtitleDelay) + 0.1
            mpv.setProperty(MpvProperties.SubtitleDelay, delay)
            osd.message(i18nc("@info:tooltip", "Subtitle timing: %1", delay.toFixed(2)))
        }

        onSubtitleToggleAction: {
            const visible = mpv.getProperty(MpvProperties.SubtitleVisibility)
            const message = visible ? i18nc("@info:tooltip", "Subtitles off") : i18nc("@info:tooltip", "Subtitles on")
            mpv.setProperty(MpvProperties.SubtitleVisibility, !visible)
            osd.message(message)
        }

        onSubtitleCycleUpAction: {
            mpv.synchronousCommand(["cycle", "sid", "up"])
            const currentTrackId = mpv.getProperty(MpvProperties.SubtitleId)
            if (currentTrackId === false) {
                osd.message(i18nc("@info:tooltip", "Subtitle: None"))
            } else {
                const tracks = mpv.getProperty(MpvProperties.TrackList)
                const track = tracks.find(t => t.type === "sub" && t.id === currentTrackId)
                osd.message(i18nc("@info:tooltip", "Subtitle: %1 %2", currentTrackId, track.lang || ""))
            }
        }

        onSubtitleCycleDownAction: {
            mpv.synchronousCommand(["cycle", "sid", "down"])
            const currentTrackId = mpv.getProperty(MpvProperties.SubtitleId)
            if (currentTrackId === false) {
                osd.message(i18nc("@info:tooltip", "Subtitle: None"))
            } else {
                const tracks = mpv.getProperty(MpvProperties.TrackList)
                const track = tracks.find(t => t.type === "sub" && t.id === currentTrackId)
                osd.message(i18nc("@info:tooltip", "Subtitle: %1 %2", currentTrackId, track.lang || ""))
            }
        }

        onSubtitleIncreaseFontSizeAction: {
            const subScale = mpv.getProperty(MpvProperties.SubtitleScale) + 0.1
            mpv.setProperty(MpvProperties.SubtitleScale, subScale)
            osd.message(i18nc("@info:tooltip", "Subtitle scale: %1", subScale.toFixed(1)))
        }

        onSubtitleDecreaseFontSizeAction: {
            const subScale = mpv.getProperty(MpvProperties.SubtitleScale) - 0.1
            mpv.setProperty(MpvProperties.SubtitleDelay, subScale)
            osd.message(i18nc("@info:tooltip", "Subtitle scale: %1", subScale.toFixed(1)))
        }

        onSubtitleMoveUpAction: mpv.commandAsync(["add", "sub-pos", "-1"])

        onSubtitleMoveDownAction: mpv.commandAsync(["add", "sub-pos", "+1"])

        onToggleDeinterlacingAction: {
            const deinterlaced = !mpv.getProperty(MpvProperties.Deinterlace)
            mpv.setProperty(MpvProperties.Deinterlace, deinterlaced)
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
            const pan = mpv.getProperty(MpvProperties.VideoPanX) - 0.01
            mpv.setProperty(MpvProperties.VideoPanX, pan)
        }

        onVideoPanXRightAction: {
            const pan = mpv.getProperty(MpvProperties.VideoPanX) + 0.01
            mpv.setProperty(MpvProperties.VideoPanX, pan)
        }

        onVideoPanYUpAction: {
            const pan = mpv.getProperty(MpvProperties.VideoPanX) - 0.01
            mpv.setProperty(MpvProperties.VideoPanY, pan)
        }

        onVideoPanYDownAction: {
            const pan = mpv.getProperty(MpvProperties.VideoPanX) + 0.01
            mpv.setProperty(MpvProperties.VideoPanY, pan)
        }

        onVolumeUpAction: {
            mpv.commandAsync(["add", "volume", GeneralSettings.volumeStep])
        }

        onVolumeDownAction: {
            mpv.commandAsync(["add", "volume", -GeneralSettings.volumeStep])
        }

        onZoomInAction: {
            const zoom = mpv.getProperty(MpvProperties.VideoZoom) + 0.1
            mpv.setProperty(MpvProperties.VideoZoom, zoom)
            osd.message(i18nc("@info:tooltip", "Zoom: %1", zoom.toFixed(2)))
        }

        onZoomOutAction: {
            const zoom = mpv.getProperty(MpvProperties.VideoZoom) - 0.1
            mpv.setProperty(MpvProperties.VideoZoom, zoom)
            osd.message(i18nc("@info:tooltip", "Zoom: %1", zoom.toFixed(2)))
        }

        onZoomResetAction: {
            mpv.setProperty(MpvProperties.VideoZoom, 0)
            osd.message(i18nc("@info:tooltip", "Zoom: 0"))
        }


        onContrastUpAction: {
            const contrast = parseInt(mpv.getProperty(MpvProperties.Contrast)) + 1
            mpv.setProperty(MpvProperties.Contrast, `${contrast}`)
            osd.message(i18nc("@info:tooltip", "Contrast: %1", contrast))
        }

        onContrastDownAction: {
            const contrast = parseInt(mpv.getProperty(MpvProperties.Contrast)) - 1
            mpv.setProperty(MpvProperties.Contrast, `${contrast}`)
            osd.message(i18nc("@info:tooltip", "Contrast: %1", contrast))
        }

        onContrastResetAction: {
            mpv.setProperty(MpvProperties.Contrast, 0)
            osd.message(i18nc("@info:tooltip", "Contrast: 0"))
        }

        onBrightnessUpAction: {
            const brightness = parseInt(mpv.getProperty(MpvProperties.Brightness)) + 1
            mpv.setProperty(MpvProperties.Brightness, `${brightness}`)
            osd.message(i18nc("@info:tooltip", "Brightness: %1", brightness))
        }

        onBrightnessDownAction: {
            const brightness = parseInt(mpv.getProperty(MpvProperties.Brightness)) - 1
            mpv.setProperty(MpvProperties.Brightness, `${brightness}`)
            osd.message(i18nc("@info:tooltip", "Brightness: %1", brightness))
        }

        onBrightnessResetAction: {
            mpv.setProperty(MpvProperties.Brightness, 0)
            osd.message(i18nc("@info:tooltip", "Brightness: 0"))
        }

        onGammaUpAction: {
            const gamma = parseInt(mpv.getProperty(MpvProperties.Gamma)) + 1
            mpv.setProperty(MpvProperties.Gamma, `${gamma}`)
            osd.message(i18nc("@info:tooltip", "Gamma: %1", gamma))
        }

        onGammaDownAction: {
            const gamma = parseInt(mpv.getProperty(MpvProperties.Gamma)) - 1
            mpv.setProperty(MpvProperties.Gamma, `${gamma}`)
            osd.message(i18nc("@info:tooltip", "Gamma: %1", gamma))
        }

        onGammaResetAction: {
            mpv.setProperty(MpvProperties.Gamma, 0)
            osd.message(i18nc("@info:tooltip", "Gamma: 0"))
        }

        onSaturationUpAction: {
            const saturation = parseInt(mpv.getProperty(MpvProperties.Saturation)) + 1
            mpv.setProperty(MpvProperties.Saturation, `${saturation}`)
            osd.message(i18nc("@info:tooltip", "Saturation: %1", saturation))
        }

        onSaturationDownAction: {
            const saturation = parseInt(mpv.getProperty(MpvProperties.Saturation)) - 1
            mpv.setProperty(MpvProperties.Saturation, `${saturation}`)
            osd.message(i18nc("@info:tooltip", "Saturation: %1", saturation))
        }

        onSaturationResetAction: {
            mpv.setProperty(MpvProperties.Saturation, 0)
            osd.message(i18nc("@info:tooltip", "Saturation: 0"))
        }
    }
}
