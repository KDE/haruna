/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import Qt.labs.platform as Labs

import org.kde.haruna

Labs.Menu {
    id: root

    title: i18nc("@title:menu", "&Playback")

    Labs.MenuItem {
        icon.name: HarunaApp.actions.playPauseAction.icon.name
        text: HarunaApp.actions.playPauseAction.text
        onTriggered: HarunaApp.actions.playPauseAction.trigger()
    }

    Labs.MenuItem {
        icon.name: HarunaApp.actions.playNextAction.icon.name
        text: HarunaApp.actions.playNextAction.text
        onTriggered: HarunaApp.actions.playNextAction.trigger()
    }

    Labs.MenuItem {
        icon.name: HarunaApp.actions.playPreviousAction.icon.name
        text: HarunaApp.actions.playPreviousAction.text
        onTriggered: HarunaApp.actions.playPreviousAction.trigger()
    }

    Labs.MenuItem {
        icon.name: HarunaApp.actions.restartPlaybackAction.icon.name
        text: HarunaApp.actions.restartPlaybackAction.text
        onTriggered: HarunaApp.actions.restartPlaybackAction.trigger()
    }

    Labs.MenuItem {
        icon.name: HarunaApp.actions.setLoopAction.icon.name
        text: HarunaApp.actions.setLoopAction.text
        onTriggered: HarunaApp.actions.setLoopAction.trigger()
    }

    Labs.MenuSeparator {}

    Labs.MenuItem {
        icon.name: HarunaApp.actions.playbackSpeedIncreaseAction.icon.name
        text: HarunaApp.actions.playbackSpeedIncreaseAction.text
        onTriggered: HarunaApp.actions.playbackSpeedIncreaseAction.trigger()
    }

    Labs.MenuItem {
        icon.name: HarunaApp.actions.playbackSpeedDecreaseAction.icon.name
        text: HarunaApp.actions.playbackSpeedDecreaseAction.text
        onTriggered: HarunaApp.actions.playbackSpeedDecreaseAction.trigger()
    }

    Labs.MenuItem {
        icon.name: HarunaApp.actions.playbackSpeedResetAction.icon.name
        text: HarunaApp.actions.playbackSpeedResetAction.text
        onTriggered: HarunaApp.actions.playbackSpeedResetAction.trigger()
    }

    Labs.MenuSeparator {}

    Labs.Menu {
        title: i18nc("@title:menu", "Seek")
        Labs.MenuItem {
            icon.name: HarunaApp.actions.seekForwardSmallAction.icon.name
            text: HarunaApp.actions.seekForwardSmallAction.text
            onTriggered: HarunaApp.actions.seekForwardSmallAction.trigger()
        }
        Labs.MenuItem {
            icon.name: HarunaApp.actions.seekBackwardSmallAction.icon.name
            text: HarunaApp.actions.seekBackwardSmallAction.text
            onTriggered: HarunaApp.actions.seekBackwardSmallAction.trigger()
        }

        Labs.MenuSeparator {}

        Labs.MenuItem {
            icon.name: HarunaApp.actions.seekForwardMediumAction.icon.name
            text: HarunaApp.actions.seekForwardMediumAction.text
            onTriggered: HarunaApp.actions.seekForwardMediumAction.trigger()
        }

        Labs.MenuItem {
            icon.name: HarunaApp.actions.seekBackwardMediumAction.icon.name
            text: HarunaApp.actions.seekBackwardMediumAction.text
            onTriggered: HarunaApp.actions.seekBackwardMediumAction.trigger()
        }

        Labs.MenuSeparator {}

        Labs.MenuItem {
            icon.name: HarunaApp.actions.seekForwardBigAction.icon.name
            text: HarunaApp.actions.seekForwardBigAction.text
            onTriggered: HarunaApp.actions.seekForwardBigAction.trigger()
        }

        Labs.MenuItem {
            icon.name: HarunaApp.actions.seekBackwardBigAction.icon.name
            text: HarunaApp.actions.seekBackwardBigAction.text
            onTriggered: HarunaApp.actions.seekBackwardBigAction.trigger()
        }

        Labs.MenuSeparator {}

        Labs.MenuItem {
            icon.name: HarunaApp.actions.seekNextSubtitleAction.icon.name
            text: HarunaApp.actions.seekNextSubtitleAction.text
            onTriggered: HarunaApp.actions.seekNextSubtitleAction.trigger()
        }

        Labs.MenuItem {
            icon.name: HarunaApp.actions.seekPreviousSubtitleAction.icon.name
            text: HarunaApp.actions.seekPreviousSubtitleAction.text
            onTriggered: HarunaApp.actions.seekPreviousSubtitleAction.trigger()
        }

        Labs.MenuSeparator {}

        Labs.MenuItem {
            icon.name: HarunaApp.actions.seekNextChapterAction.icon.name
            text: HarunaApp.actions.seekNextChapterAction.text
            onTriggered: HarunaApp.actions.seekNextChapterAction.trigger()
        }

        Labs.MenuItem {
            icon.name: HarunaApp.actions.seekPreviousChapterAction.icon.name
            text: HarunaApp.actions.seekPreviousChapterAction.text
            onTriggered: HarunaApp.actions.seekPreviousChapterAction.trigger()
        }

        Labs.MenuSeparator {}

        Labs.MenuItem {
            icon.name: HarunaApp.actions.frameStepForwardAction.icon.name
            text: HarunaApp.actions.frameStepForwardAction.text
            onTriggered: HarunaApp.actions.frameStepForwardAction.trigger()
        }

        Labs.MenuItem {
            icon.name: HarunaApp.actions.frameStepBackwardAction.icon.name
            text: HarunaApp.actions.frameStepBackwardAction.text
            onTriggered: HarunaApp.actions.frameStepBackwardAction.trigger()
        }

        Labs.MenuItem {
            icon.name: HarunaApp.actions.seekToWatchLaterPositionAction.icon.name
            text: HarunaApp.actions.seekToWatchLaterPositionAction.text
            onTriggered: HarunaApp.actions.seekToWatchLaterPositionAction.trigger()
        }
    }
}
