/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

import org.kde.haruna
import org.kde.haruna.utilities
import org.kde.haruna.settings

Item {
    id: root

    required property ActionsModel m_actionsModel
    required property MpvItem m_mpv
    required property Loader m_mpvContextMenuLoader
    required property Loader m_settingsLoader
    required property Osd m_osd
    required property SelectActionPopup m_triggerActionPopup
    required property InputPopup m_openUrlPopup

    signal openFileDialog()
    signal openSubtitleDialog()
    signal togglePlaylist()

    Instantiator {
        model: root.m_actionsModel
        delegate: Action {
            id: delegate

            required property string actionName
            required property string actionText
            required property string actionDescription
            required property string actionShortcut
            required property string actionIcon
            required property string actionType

            enabled: HarunaApp.actionsEnabled
            objectName: delegate.actionName
            text: delegate.actionText
            shortcut: delegate.actionShortcut
            icon.name: delegate.actionIcon
            onTriggered: {
                if (delegate.actionType === "NormalAction") {
                    root.m_actionsModel.signalEmitter(objectName)
                }
                if (delegate.actionType === "CustomAction") {
                    root.m_mpv.userCommand(delegate.actionText)
                    root.m_osd.message(root.m_mpv.expandText(delegate.actionDescription))
                }
            }

            Component.onCompleted: HarunaApp.actions[objectName] = this
        }
    }

    Connections {
        target: root.m_actionsModel

        function onOpenActionsDialogAction() {
            root.m_triggerActionPopup.open()
        }

        function onAboutHarunaAction() {
            root.m_settingsLoader.openSettingPage(SettingsWindow.Page.About)
        }

        function onReportBugAction() {
            Qt.openUrlExternally("https://bugs.kde.org/enter_bug.cgi?product=Haruna")
        }

        function onSponsorAction() {
            Qt.openUrlExternally("https://github.com/sponsors/g-fb")
        }

        function onAudioCycleUpAction() {
            const tracks = root.m_mpv.getProperty(MpvProperties.TrackList)
            let audioTracksCount = 0
            tracks.forEach(t => { if(t.type === "audio") ++audioTracksCount })

            if (audioTracksCount > 1) {
                root.m_mpv.command(["cycle", "aid", "up"])
                const currentTrackId = root.m_mpv.getProperty(MpvProperties.AudioId)

                if (currentTrackId === false) {
                    root.m_actionsModel.signalEmitter("audioCycleUpAction")
                    return
                }
                const track = tracks.find(t => t.type === "audio" && t.id === currentTrackId)
                root.m_osd.message(i18nc("@info:tooltip; %1 is an ID, %2 is a language code (e.g. eng)", "Audio: %1 %2", currentTrackId, track.lang || ""))
            }
        }

        function onAudioCycleDownAction() {
            const tracks = root.m_mpv.getProperty(MpvProperties.TrackList)
            let audioTracksCount = 0
            tracks.forEach(t => { if(t.type === "audio") ++audioTracksCount })

            if (audioTracksCount > 1) {
                root.m_mpv.command(["cycle", "aid", "down"])
                const currentTrackId = root.m_mpv.getProperty(MpvProperties.AudioId)

                if (currentTrackId === false) {
                    root.m_actionsModel.signalEmitter("audioCycleDownAction")
                    return
                }
                const track = tracks.find(t => t.type === "audio" && t.id === currentTrackId)
                root.m_osd.message(i18nc("@info:tooltip; %1 is an ID, %2 is a language code (e.g. eng)", "Audio: %1 %2", currentTrackId, track.lang || ""))
            }
        }

        function onConfigureAction() {
            root.m_settingsLoader.openSettingPage(root.m_settingsLoader.page)
        }

        function onConfigureShortcutsAction() {
            root.m_settingsLoader.openSettingPage(SettingsWindow.Page.Shortcuts)
        }

        function onExitFullscreenAction() {
            const mainWindow = root.Window.window as Main
            mainWindow.exitFullscreen()
        }

        function onFrameStepForwardAction() {
            root.m_mpv.command(["frame-step"])
        }

        function onFrameStepBackwardAction() {
            root.m_mpv.command(["frame-back-step"])
        }

        function onLoadLastPlayedFileAction() {
            if (GeneralSettings.lastPlayedFile === "") {
                return
            }

            const mainWindow = root.Window.window as Main
            mainWindow.openFile(GeneralSettings.lastPlayedFile, RecentFilesModel.OpenedFrom.Other)
        }

        function onMuteAction() {
            root.m_mpv.mute = !root.m_mpv.mute
        }

        function onOpenContextMenuAction() {
            // if menu is open it will be closed because of the Popup.CloseOnPressOutside close policy
            // but that happens after the call to openMpvContextMenuLoader() below
            // resulting in the menu being opened for a very short time and then closed
            root.m_mpvContextMenuLoader.closeMpvContextMenuLoader()
            root.m_mpvContextMenuLoader.openMpvContextMenuLoader()
        }

        function onOpenFileAction() {
            root.openFileDialog()
        }

        function onAddExternalSubtitleAction() {
            if (root.m_mpv.currentUrl.toString() === "") {
                const osdMsg = i18nc("osd message when trying to open subtitle selection dialog while no file is open",
                                     "Can’t load subtitle when no file is open")
                root.m_osd.message(osdMsg)
                return
            }

            root.openSubtitleDialog()
        }

        function onOpenUrlAction() {
            if (root.m_openUrlPopup.visible) {
                root.m_openUrlPopup.close()
            } else {
                root.m_openUrlPopup.open()
            }
        }

        function onPlaybackSpeedIncreaseAction() {
            const speed = root.m_mpv.getProperty(MpvProperties.Speed) + 0.1
            root.m_mpv.setProperty(MpvProperties.Speed, speed)
            root.m_osd.message(i18nc("@info:tooltip", "Playback speed: %1", speed.toFixed(2)))
        }

        function onPlaybackSpeedDecreaseAction() {
            const speed = root.m_mpv.getProperty(MpvProperties.Speed) - 0.1
            root.m_mpv.setProperty(MpvProperties.Speed, speed)
            root.m_osd.message(i18nc("@info:tooltip", "Playback speed: %1", speed.toFixed(2)))
        }

        function onPlaybackSpeedResetAction() {
            root.m_mpv.setProperty(MpvProperties.Speed, 1.0)
            root.m_osd.message(i18nc("@info:tooltip", "Playback speed: %1", 1.0))
        }

        function onPlayPauseAction() {
            root.m_mpv.pause = !root.m_mpv.pause
        }

        function onPlayNextAction() {
            root.m_mpv.activeFilterProxyModel.playNext()
        }

        function onPlayPreviousAction() {
            root.m_mpv.activeFilterProxyModel.playPrevious()
        }

        function onQuitApplicationAction() {
            Qt.quit()
        }

        function onRestartPlaybackAction() {
            root.m_mpv.command(["seek", 0, "absolute"])
        }

        function onSeekForwardSmallAction() {
            root.m_mpv.command(["seek", PlaybackSettings.seekSmallStep, "exact"])
        }

        function onSeekBackwardSmallAction() {
            root.m_mpv.command(["seek", -PlaybackSettings.seekSmallStep, "exact"])
        }

        function onSeekForwardMediumAction() {
            root.m_mpv.command(["seek", PlaybackSettings.seekMediumStep, "exact"])
        }

        function onSeekBackwardMediumAction() {
            root.m_mpv.command(["seek", -PlaybackSettings.seekMediumStep, "exact"])
        }

        function onSeekForwardBigAction() {
            root.m_mpv.command(["seek", PlaybackSettings.seekBigStep, "exact"])
        }

        function onSeekBackwardBigAction() {
            root.m_mpv.command(["seek", -PlaybackSettings.seekBigStep, "exact"])
        }

        function onSeekNextChapterAction() {
            const chapters = root.m_mpv.getProperty(MpvProperties.ChapterList)
            const currentChapter = root.m_mpv.getProperty(MpvProperties.Chapter)
            const nextChapter = currentChapter + 1
            if (nextChapter === chapters.length) {
                root.m_actionsModel.signalEmitter("playNextAction")
                return
            }
            root.m_mpv.command(["add", "chapter", "1"])
        }

        function onSeekPreviousChapterAction() {
            root.m_mpv.command(["add", "chapter", "-1"])
        }

        function onSeekNextSubtitleAction() {
            if (root.m_mpv.getProperty(MpvProperties.SubtitleId) !== false) {
                root.m_mpv.command(["sub-seek", "1"])
            } else {
                root.m_actionsModel.signalEmitter("seekForwardSmallAction")
            }
        }

        function onSeekPreviousSubtitleAction() {
            if (root.m_mpv.getProperty(MpvProperties.SubtitleId) !== false) {
                root.m_mpv.command(["sub-seek", "-1"])
            } else {
                root.m_actionsModel.signalEmitter("seekBackwardSmallAction")
            }
        }

        function onSeekToWatchLaterPositionAction() {
            if (root.m_mpv.watchLaterPosition === 0) {
                return
            }
            root.m_mpv.command(["seek", root.m_mpv.watchLaterPosition, "absolute"])
        }

        function onSetLoopAction() {
            var a = root.m_mpv.getProperty(MpvProperties.ABLoopA)
            var b = root.m_mpv.getProperty(MpvProperties.ABLoopB)

            var aIsSet = a !== "no"
            var bIsSet = b !== "no"

            if (!aIsSet && !bIsSet) {
                root.m_mpv.setProperty(MpvProperties.ABLoopA, root.m_mpv.position)
                footer.progressBar.loopIndicator.startPosition = root.m_mpv.position
                root.m_osd.message(i18nc("@info:tooltip; %1 is the timestamp where the loop starts", "Loop start: %1", MiscUtils.formatTime(root.m_mpv.position)))
            } else if (aIsSet && !bIsSet) {
                // set b position slightly ahead to ensure the loop section is not skipped
                const bPosition = root.m_mpv.position + 0.1
                root.m_mpv.setPropertyBlocking(MpvProperties.ABLoopB, bPosition)
                footer.progressBar.loopIndicator.endPosition = bPosition
                root.m_osd.message(i18nc("@info:tooltip, use en dash for the range", "Loop: %1–%2", MiscUtils.formatTime(a), MiscUtils.formatTime(bPosition)))
            } else {
                root.m_mpv.setProperty(MpvProperties.ABLoopA, "no")
                root.m_mpv.setProperty(MpvProperties.ABLoopB, "no")
                footer.progressBar.loopIndicator.startPosition = -1
                footer.progressBar.loopIndicator.endPosition = -1
                root.m_osd.message(i18nc("@info:tooltip", "Loop cleared"))
            }
        }

        function onScreenshotAction() {
            root.m_mpv.commandAsync(["screenshot"], MpvItem.Screenshot)
        }

        function onSubtitleQuickenAction() {
            const delay = root.m_mpv.getProperty(MpvProperties.SubtitleDelay) - 0.1
            root.m_mpv.setProperty(MpvProperties.SubtitleDelay, delay)
        }

        function onSubtitleDelayAction() {
            const delay = root.m_mpv.getProperty(MpvProperties.SubtitleDelay) + 0.1
            root.m_mpv.setProperty(MpvProperties.SubtitleDelay, delay)
        }

        function onSubtitleToggleAction() {
            const visible = root.m_mpv.getProperty(MpvProperties.SubtitleVisibility)
            const message = visible ? i18nc("@info:tooltip", "Subtitles off") : i18nc("@info:tooltip", "Subtitles on")
            root.m_mpv.setProperty(MpvProperties.SubtitleVisibility, !visible)
            root.m_osd.message(message)
        }

        function onSubtitleCycleUpAction() {
            root.m_mpv.command(["cycle", "sid", "up"])
            const currentTrackId = root.m_mpv.getProperty(MpvProperties.SubtitleId)
            if (currentTrackId === false) {
                root.m_osd.message(i18nc("@info:tooltip", "Subtitle: None"))
            } else {
                const tracks = root.m_mpv.getProperty(MpvProperties.TrackList)
                const track = tracks.find(t => t.type === "sub" && t.id === currentTrackId)
                root.m_osd.message(i18nc("@info:tooltip; %1 is an ID, %2 is a language code (e.g. eng)", "Subtitle: %1 %2", currentTrackId, track.lang || ""))
            }
        }

        function onSubtitleCycleDownAction() {
            root.m_mpv.command(["cycle", "sid", "down"])
            const currentTrackId = root.m_mpv.getProperty(MpvProperties.SubtitleId)
            if (currentTrackId === false) {
                root.m_osd.message(i18nc("@info:tooltip", "Subtitle: None"))
            } else {
                const tracks = root.m_mpv.getProperty(MpvProperties.TrackList)
                const track = tracks.find(t => t.type === "sub" && t.id === currentTrackId)
                root.m_osd.message(i18nc("@info:tooltip; %1 is an ID, %2 is a language code (e.g. eng)", "Subtitle: %1 %2", currentTrackId, track.lang || ""))
            }
        }

        function onSubtitleIncreaseFontSizeAction() {
            const subScale = root.m_mpv.getProperty(MpvProperties.SubtitleScale) + 0.1
            root.m_mpv.setProperty(MpvProperties.SubtitleScale, subScale)
            root.m_osd.message(i18nc("@info:tooltip", "Subtitle scale: %1", subScale.toFixed(1)))
        }

        function onSubtitleDecreaseFontSizeAction() {
            const subScale = root.m_mpv.getProperty(MpvProperties.SubtitleScale) - 0.1
            root.m_mpv.setProperty(MpvProperties.SubtitleScale, subScale)
            root.m_osd.message(i18nc("@info:tooltip", "Subtitle scale: %1", subScale.toFixed(1)))
        }

        function onSubtitleMoveUpAction() {
            root.m_mpv.command(["add", "sub-pos", "-1"])
        }

        function onSubtitleMoveDownAction() {
            root.m_mpv.command(["add", "sub-pos", "+1"])
        }

        function onToggleDeinterlacingAction() {
            const deinterlaced = !root.m_mpv.getProperty(MpvProperties.Deinterlace)
            root.m_mpv.setProperty(MpvProperties.Deinterlace, deinterlaced)
            root.m_osd.message(i18nc("@info:tooltip", "Deinterlace: %1", deinterlaced))
        }

        function onToggleFullscreenAction() {
            const mainWindow = root.Window.window as Main
            mainWindow.toggleFullScreen()
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
            root.togglePlaylist()
        }

        function onVideoPanXLeftAction() {
            const pan = root.m_mpv.getProperty(MpvProperties.VideoPanX) - 0.01
            root.m_mpv.setProperty(MpvProperties.VideoPanX, pan)
        }

        function onVideoPanXRightAction() {
            const pan = root.m_mpv.getProperty(MpvProperties.VideoPanX) + 0.01
            root.m_mpv.setProperty(MpvProperties.VideoPanX, pan)
        }

        function onVideoPanYUpAction() {
            const pan = root.m_mpv.getProperty(MpvProperties.VideoPanY) - 0.01
            root.m_mpv.setProperty(MpvProperties.VideoPanY, pan)
        }

        function onVideoPanYDownAction() {
            const pan = root.m_mpv.getProperty(MpvProperties.VideoPanY) + 0.01
            root.m_mpv.setProperty(MpvProperties.VideoPanY, pan)
        }

        function onRotateClockwiseAction() {
            root.m_mpv.commandAsync(["cycle_values", "video-rotate", "90", "180", "270", "0"])
        }

        function onRotateCounterClockwiseAction() {
            root.m_mpv.commandAsync(["cycle_values", "video-rotate", "270", "180", "90", "0"])
        }

        function onVolumeUpAction() {
            root.m_mpv.command(["add", "volume", AudioSettings.volumeStep])
        }

        function onVolumeDownAction() {
            root.m_mpv.command(["add", "volume", -AudioSettings.volumeStep])
        }

        function onZoomInAction() {
            const zoom = root.m_mpv.getProperty(MpvProperties.VideoZoom) + 0.1
            root.m_mpv.setProperty(MpvProperties.VideoZoom, zoom)
            root.m_osd.message(i18nc("@info:tooltip", "Zoom: %1", zoom.toFixed(2)))
        }

        function onZoomOutAction() {
            const zoom = root.m_mpv.getProperty(MpvProperties.VideoZoom) - 0.1
            root.m_mpv.setProperty(MpvProperties.VideoZoom, zoom)
            root.m_osd.message(i18nc("@info:tooltip", "Zoom: %1", zoom.toFixed(2)))
        }

        function onZoomResetAction() {
            root.m_mpv.setProperty(MpvProperties.VideoZoom, 0)
            root.m_osd.message(i18nc("@info:tooltip", "Zoom: 0"))
        }


        function onContrastUpAction() {
            const contrast = parseInt(root.m_mpv.getProperty(MpvProperties.Contrast)) + 1
            root.m_mpv.setProperty(MpvProperties.Contrast, `${contrast}`)
            root.m_osd.message(i18nc("@info:tooltip", "Contrast: %1", contrast))
        }

        function onContrastDownAction() {
            const contrast = parseInt(root.m_mpv.getProperty(MpvProperties.Contrast)) - 1
            root.m_mpv.setProperty(MpvProperties.Contrast, `${contrast}`)
            root.m_osd.message(i18nc("@info:tooltip", "Contrast: %1", contrast))
        }

        function onContrastResetAction() {
            root.m_mpv.setProperty(MpvProperties.Contrast, 0)
            root.m_osd.message(i18nc("@info:tooltip", "Contrast: 0"))
        }

        function onBrightnessUpAction() {
            const brightness = parseInt(root.m_mpv.getProperty(MpvProperties.Brightness)) + 1
            root.m_mpv.setProperty(MpvProperties.Brightness, `${brightness}`)
            root.m_osd.message(i18nc("@info:tooltip", "Brightness: %1", brightness))
        }

        function onBrightnessDownAction() {
            const brightness = parseInt(root.m_mpv.getProperty(MpvProperties.Brightness)) - 1
            root.m_mpv.setProperty(MpvProperties.Brightness, `${brightness}`)
            root.m_osd.message(i18nc("@info:tooltip", "Brightness: %1", brightness))
        }

        function onBrightnessResetAction() {
            root.m_mpv.setProperty(MpvProperties.Brightness, 0)
            root.m_osd.message(i18nc("@info:tooltip", "Brightness: 0"))
        }

        function onGammaUpAction() {
            const gamma = parseInt(root.m_mpv.getProperty(MpvProperties.Gamma)) + 1
            root.m_mpv.setProperty(MpvProperties.Gamma, `${gamma}`)
            root.m_osd.message(i18nc("@info:tooltip", "Gamma: %1", gamma))
        }

        function onGammaDownAction() {
            const gamma = parseInt(root.m_mpv.getProperty(MpvProperties.Gamma)) - 1
            root.m_mpv.setProperty(MpvProperties.Gamma, `${gamma}`)
            root.m_osd.message(i18nc("@info:tooltip", "Gamma: %1", gamma))
        }

        function onGammaResetAction() {
            root.m_mpv.setProperty(MpvProperties.Gamma, 0)
            root.m_osd.message(i18nc("@info:tooltip", "Gamma: 0"))
        }

        function onSaturationUpAction() {
            const saturation = parseInt(root.m_mpv.getProperty(MpvProperties.Saturation)) + 1
            root.m_mpv.setProperty(MpvProperties.Saturation, `${saturation}`)
            root.m_osd.message(i18nc("@info:tooltip", "Saturation: %1", saturation))
        }

        function onSaturationDownAction() {
            const saturation = parseInt(root.m_mpv.getProperty(MpvProperties.Saturation)) - 1
            root.m_mpv.setProperty(MpvProperties.Saturation, `${saturation}`)
            root.m_osd.message(i18nc("@info:tooltip", "Saturation: %1", saturation))
        }

        function onSaturationResetAction() {
            root.m_mpv.setProperty(MpvProperties.Saturation, 0)
            root.m_osd.message(i18nc("@info:tooltip", "Saturation: 0"))
        }
    }
}
