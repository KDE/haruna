/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls

import org.kde.haruna
import org.kde.haruna.mpvproperties
import org.kde.haruna.settings
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

        function onOpenActionsDialogAction() {
            triggerActionPopup.open()
        }

        function onAboutHarunaAction() {
            settingsLoader.active = true
            settingsLoader.item.currentPage = SettingsWindow.Page.About
            actionsModel.signalEmitter("configureAction")
        }

        function onAudioCycleUpAction() {
            const tracks = mpv.getProperty(MpvProperties.TrackList)
            let audioTracksCount = 0
            tracks.forEach(t => { if(t.type === "audio") ++audioTracksCount })

            if (audioTracksCount > 1) {
                mpv.command(["cycle", "aid", "up"])
                const currentTrackId = mpv.getProperty(MpvProperties.AudioId)

                if (currentTrackId === false) {
                    actionsModel.signalEmitter("audioCycleUpAction")
                    return
                }
                const track = tracks.find(t => t.type === "audio" && t.id === currentTrackId)
                osd.message(i18nc("@info:tooltip", "Audio: %1 %2", currentTrackId, track.lang || ""))
            }
        }

        function onAudioCycleDownAction() {
            const tracks = mpv.getProperty(MpvProperties.TrackList)
            let audioTracksCount = 0
            tracks.forEach(t => { if(t.type === "audio") ++audioTracksCount })

            if (audioTracksCount > 1) {
                mpv.command(["cycle", "aid", "down"])
                const currentTrackId = mpv.getProperty(MpvProperties.AudioId)

                if (currentTrackId === false) {
                    actionsModel.signalEmitter("audioCycleDownAction")
                    return
                }
                const track = tracks.find(t => t.type === "audio" && t.id === currentTrackId)
                osd.message(i18nc("@info:tooltip", "Audio: %1 %2", currentTrackId, track.lang || ""))
            }
        }

        function onConfigureAction() {
            settingsLoader.active = true
            if (settingsLoader.item.visible) {
                settingsLoader.item.raise()
            } else {
                settingsLoader.item.visible = true
            }
        }

        function onConfigureShortcutsAction() {
            settingsLoader.active = true
            settingsLoader.item.currentPage = SettingsWindow.Page.Shortcuts
            actionsModel.signalEmitter("configureAction")
        }

        function onExitFullscreenAction() {
            window.exitFullscreen()
        }

        function onFrameStepForwardAction() {
            mpv.command(["frame-step"])
        }

        function onFrameStepBackwardAction() {
            mpv.command(["frame-back-step"])
        }

        function onLoadLastPlayedFileAction() {
            window.openFile(GeneralSettings.lastPlayedFile)
        }

        function onMuteAction() {
            mpv.mute = !mpv.mute
        }

        function onOpenContextMenuAction() {
            mpvContextMenuLoader.active = true
            mpvContextMenuLoader.item.popup()
        }

        function onOpenFileAction() {
            fileDialog.open()
        }

        function onOpenSubtitlesFileAction() {
            subtitlesFileDialog.open()
        }

        function onOpenUrlAction() {
            if (openUrlPopup.visible) {
                openUrlPopup.close()
            } else {
                openUrlPopup.open()
            }
        }

        function onPlaybackSpeedIncreaseAction() {
            const speed = mpv.getProperty(MpvProperties.Speed) + 0.1
            mpv.setProperty(MpvProperties.Speed, speed)
            osd.message(i18nc("@info:tooltip", "Playback speed: %1", speed.toFixed(2)))
        }

        function onPlaybackSpeedDecreaseAction() {
            const speed = mpv.getProperty(MpvProperties.Speed) - 0.1
            mpv.setProperty(MpvProperties.Speed, speed)
            osd.message(i18nc("@info:tooltip", "Playback speed: %1", speed.toFixed(2)))
        }

        function onPlaybackSpeedResetAction() {
            mpv.setProperty(MpvProperties.Speed, 1.0)
            osd.message(i18nc("@info:tooltip", "Playback speed: %1", 1.0))
        }

        function onPlayPauseAction() {
            mpv.pause = !mpv.pause
        }

        function onPlayNextAction() {
            mpv.playlistProxyModel.playNext()
        }

        function onPlayPreviousAction() {
            mpv.playlistProxyModel.playPrevious()
        }

        function onQuitApplicationAction() {
            Qt.quit()
        }

        function onRestartPlaybackAction() {
            mpv.command(["seek", 0, "absolute"])
        }

        function onSeekForwardSmallAction() {
            mpv.command(["seek", PlaybackSettings.seekSmallStep, "exact"])
        }

        function onSeekBackwardSmallAction() {
            mpv.command(["seek", -PlaybackSettings.seekSmallStep, "exact"])
        }

        function onSeekForwardMediumAction() {
            mpv.command(["seek", PlaybackSettings.seekMediumStep, "exact"])
        }

        function onSeekBackwardMediumAction() {
            mpv.command(["seek", -PlaybackSettings.seekMediumStep, "exact"])
        }

        function onSeekForwardBigAction() {
            mpv.command(["seek", PlaybackSettings.seekBigStep, "exact"])
        }

        function onSeekBackwardBigAction() {
            mpv.command(["seek", -PlaybackSettings.seekBigStep, "exact"])
        }

        function onSeekNextChapterAction() {
            const chapters = mpv.getProperty(MpvProperties.ChapterList)
            const currentChapter = mpv.getProperty(MpvProperties.Chapter)
            const nextChapter = currentChapter + 1
            if (nextChapter === chapters.length) {
                actionsModel.signalEmitter("playNextAction")
                return
            }
            mpv.command(["add", "chapter", "1"])
        }

        function onSeekPreviousChapterAction() {
            mpv.command(["add", "chapter", "-1"])
        }

        function onSeekNextSubtitleAction() {
            if (mpv.getProperty(MpvProperties.SubtitleId) !== false) {
                mpv.command(["sub-seek", "1"])
            } else {
                actionsModel.signalEmitter("seekForwardSmallAction")
            }
        }

        function onSeekPreviousSubtitleAction() {
            if (mpv.getProperty(MpvProperties.SubtitleId) !== false) {
                mpv.command(["sub-seek", "-1"])
            } else {
                actionsModel.signalEmitter("seekBackwardSmallAction")
            }
        }

        function onSeekToWatchLaterPositionAction() {
            if (mpv.watchLaterPosition === 0) {
                return
            }
            mpv.command(["seek", mpv.watchLaterPosition, "absolute"])
        }

        function onSetLoopAction() {
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

        function onScreenshotAction() {
            mpv.commandAsync(["screenshot"], MpvItem.Screenshot)
        }

        function onSubtitleQuickenAction() {
            const delay = mpv.getProperty(MpvProperties.SubtitleDelay) - 0.1
            mpv.setProperty(MpvProperties.SubtitleDelay, delay)
            osd.message(i18nc("@info:tooltip", "Subtitle timing: %1", delay.toFixed(2)))
        }

        function onSubtitleDelayAction() {
            const delay = mpv.getProperty(MpvProperties.SubtitleDelay) + 0.1
            mpv.setProperty(MpvProperties.SubtitleDelay, delay)
            osd.message(i18nc("@info:tooltip", "Subtitle timing: %1", delay.toFixed(2)))
        }

        function onSubtitleToggleAction() {
            const visible = mpv.getProperty(MpvProperties.SubtitleVisibility)
            const message = visible ? i18nc("@info:tooltip", "Subtitles off") : i18nc("@info:tooltip", "Subtitles function on")
            mpv.setProperty(MpvProperties.SubtitleVisibility, !visible)
            osd.message(message)
        }

        function onSubtitleCycleUpAction() {
            mpv.command(["cycle", "sid", "up"])
            const currentTrackId = mpv.getProperty(MpvProperties.SubtitleId)
            if (currentTrackId === false) {
                osd.message(i18nc("@info:tooltip", "Subtitle: None"))
            } else {
                const tracks = mpv.getProperty(MpvProperties.TrackList)
                const track = tracks.find(t => t.type === "sub" && t.id === currentTrackId)
                osd.message(i18nc("@info:tooltip", "Subtitle: %1 %2", currentTrackId, track.lang || ""))
            }
        }

        function onSubtitleCycleDownAction() {
            mpv.command(["cycle", "sid", "down"])
            const currentTrackId = mpv.getProperty(MpvProperties.SubtitleId)
            if (currentTrackId === false) {
                osd.message(i18nc("@info:tooltip", "Subtitle: None"))
            } else {
                const tracks = mpv.getProperty(MpvProperties.TrackList)
                const track = tracks.find(t => t.type === "sub" && t.id === currentTrackId)
                osd.message(i18nc("@info:tooltip", "Subtitle: %1 %2", currentTrackId, track.lang || ""))
            }
        }

        function onSubtitleIncreaseFontSizeAction() {
            const subScale = mpv.getProperty(MpvProperties.SubtitleScale) + 0.1
            mpv.setProperty(MpvProperties.SubtitleScale, subScale)
            osd.message(i18nc("@info:tooltip", "Subtitle scale: %1", subScale.toFixed(1)))
        }

        function onSubtitleDecreaseFontSizeAction() {
            const subScale = mpv.getProperty(MpvProperties.SubtitleScale) - 0.1
            mpv.setProperty(MpvProperties.SubtitleScale, subScale)
            osd.message(i18nc("@info:tooltip", "Subtitle scale: %1", subScale.toFixed(1)))
        }

        function onSubtitleMoveUpAction() {
            mpv.command(["add", "sub-pos", "-1"])
        }

        function onSubtitleMoveDownAction() {
            mpv.command(["add", "sub-pos", "+1"])
        }

        function onToggleDeinterlacingAction() {
            const deinterlaced = !mpv.getProperty(MpvProperties.Deinterlace)
            mpv.setProperty(MpvProperties.Deinterlace, deinterlaced)
            osd.message(i18nc("@info:tooltip", "Deinterlace: %1", deinterlaced))
        }

        function onToggleFullscreenAction() {
            window.toggleFullScreen()
        }

        function onToggleMenuBarAction() {
            GeneralSettings.showMenuBar = !GeneralSettings.showMenuBar
            GeneralSettings.save()
        }

        function onToggleHeaderAction() {
            GeneralSettings.showHeader = !GeneralSettings.showHeader
            GeneralSettings.save()
        }

        function onTogglePlaylistAction() {
            if (playlist.state === "visible") {
                playlist.state = "hidden"
            } else {
                playlist.state = "visible"
            }
        }

        function onVideoPanXLeftAction() {
            const pan = mpv.getProperty(MpvProperties.VideoPanX) - 0.01
            mpv.setProperty(MpvProperties.VideoPanX, pan)
        }

        function onVideoPanXRightAction() {
            const pan = mpv.getProperty(MpvProperties.VideoPanX) + 0.01
            mpv.setProperty(MpvProperties.VideoPanX, pan)
        }

        function onVideoPanYUpAction() {
            const pan = mpv.getProperty(MpvProperties.VideoPanY) - 0.01
            mpv.setProperty(MpvProperties.VideoPanY, pan)
        }

        function onVideoPanYDownAction() {
            const pan = mpv.getProperty(MpvProperties.VideoPanY) + 0.01
            mpv.setProperty(MpvProperties.VideoPanY, pan)
        }

        function onVolumeUpAction() {
            mpv.command(["add", "volume", AudioSettings.volumeStep])
        }

        function onVolumeDownAction() {
            mpv.command(["add", "volume", -AudioSettings.volumeStep])
        }

        function onZoomInAction() {
            const zoom = mpv.getProperty(MpvProperties.VideoZoom) + 0.1
            mpv.setProperty(MpvProperties.VideoZoom, zoom)
            osd.message(i18nc("@info:tooltip", "Zoom: %1", zoom.toFixed(2)))
        }

        function onZoomOutAction() {
            const zoom = mpv.getProperty(MpvProperties.VideoZoom) - 0.1
            mpv.setProperty(MpvProperties.VideoZoom, zoom)
            osd.message(i18nc("@info:tooltip", "Zoom: %1", zoom.toFixed(2)))
        }

        function onZoomResetAction() {
            mpv.setProperty(MpvProperties.VideoZoom, 0)
            osd.message(i18nc("@info:tooltip", "Zoom: 0"))
        }


        function onContrastUpAction() {
            const contrast = parseInt(mpv.getProperty(MpvProperties.Contrast)) + 1
            mpv.setProperty(MpvProperties.Contrast, `${contrast}`)
            osd.message(i18nc("@info:tooltip", "Contrast: %1", contrast))
        }

        function onContrastDownAction() {
            const contrast = parseInt(mpv.getProperty(MpvProperties.Contrast)) - 1
            mpv.setProperty(MpvProperties.Contrast, `${contrast}`)
            osd.message(i18nc("@info:tooltip", "Contrast: %1", contrast))
        }

        function onContrastResetAction() {
            mpv.setProperty(MpvProperties.Contrast, 0)
            osd.message(i18nc("@info:tooltip", "Contrast: 0"))
        }

        function onBrightnessUpAction() {
            const brightness = parseInt(mpv.getProperty(MpvProperties.Brightness)) + 1
            mpv.setProperty(MpvProperties.Brightness, `${brightness}`)
            osd.message(i18nc("@info:tooltip", "Brightness: %1", brightness))
        }

        function onBrightnessDownAction() {
            const brightness = parseInt(mpv.getProperty(MpvProperties.Brightness)) - 1
            mpv.setProperty(MpvProperties.Brightness, `${brightness}`)
            osd.message(i18nc("@info:tooltip", "Brightness: %1", brightness))
        }

        function onBrightnessResetAction() {
            mpv.setProperty(MpvProperties.Brightness, 0)
            osd.message(i18nc("@info:tooltip", "Brightness: 0"))
        }

        function onGammaUpAction() {
            const gamma = parseInt(mpv.getProperty(MpvProperties.Gamma)) + 1
            mpv.setProperty(MpvProperties.Gamma, `${gamma}`)
            osd.message(i18nc("@info:tooltip", "Gamma: %1", gamma))
        }

        function onGammaDownAction() {
            const gamma = parseInt(mpv.getProperty(MpvProperties.Gamma)) - 1
            mpv.setProperty(MpvProperties.Gamma, `${gamma}`)
            osd.message(i18nc("@info:tooltip", "Gamma: %1", gamma))
        }

        function onGammaResetAction() {
            mpv.setProperty(MpvProperties.Gamma, 0)
            osd.message(i18nc("@info:tooltip", "Gamma: 0"))
        }

        function onSaturationUpAction() {
            const saturation = parseInt(mpv.getProperty(MpvProperties.Saturation)) + 1
            mpv.setProperty(MpvProperties.Saturation, `${saturation}`)
            osd.message(i18nc("@info:tooltip", "Saturation: %1", saturation))
        }

        function onSaturationDownAction() {
            const saturation = parseInt(mpv.getProperty(MpvProperties.Saturation)) - 1
            mpv.setProperty(MpvProperties.Saturation, `${saturation}`)
            osd.message(i18nc("@info:tooltip", "Saturation: %1", saturation))
        }

        function onSaturationResetAction() {
            mpv.setProperty(MpvProperties.Saturation, 0)
            osd.message(i18nc("@info:tooltip", "Saturation: 0"))
        }
    }
}
