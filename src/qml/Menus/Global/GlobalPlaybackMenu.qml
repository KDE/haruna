/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.12
import Qt.labs.platform 1.1 as Labs

Labs.Menu {
    id: root

    title: i18n("&Playback")

    Labs.MenuItem {
        icon.name: appActions.playPauseAction.icon.name
        text: appActions.playPauseAction.text
        onTriggered: appActions.playPauseAction.trigger()
    }

    Labs.MenuItem {
        icon.name: appActions.playNextAction.icon.name
        text: appActions.playNextAction.text
        onTriggered: appActions.playNextAction.trigger()
    }

    Labs.MenuItem {
        icon.name: appActions.playPreviousAction.icon.name
        text: appActions.playPreviousAction.text
        onTriggered: appActions.playPreviousAction.trigger()
    }

    Labs.MenuItem {
        icon.name: appActions.restartPlaybackAction.icon.name
        text: appActions.restartPlaybackAction.text
        onTriggered: appActions.restartPlaybackAction.trigger()
    }

    Labs.MenuItem {
        icon.name: appActions.setLoopAction.icon.name
        text: appActions.setLoopAction.text
        onTriggered: appActions.setLoopAction.trigger()
    }

    Labs.MenuSeparator {}

    Labs.MenuItem {
        icon.name: appActions.playbackSpeedIncreaseAction.icon.name
        text: appActions.playbackSpeedIncreaseAction.text
        onTriggered: appActions.playbackSpeedIncreaseAction.trigger()
    }

    Labs.MenuItem {
        icon.name: appActions.playbackSpeedDecreaseAction.icon.name
        text: appActions.playbackSpeedDecreaseAction.text
        onTriggered: appActions.playbackSpeedDecreaseAction.trigger()
    }

    Labs.MenuItem {
        icon.name: appActions.playbackSpeedResetAction.icon.name
        text: appActions.playbackSpeedResetAction.text
        onTriggered: appActions.playbackSpeedResetAction.trigger()
    }

    Labs.MenuSeparator {}

    Labs.Menu {
        title: i18n("Seek")
        Labs.MenuItem {
            icon.name: appActions.seekForwardSmallAction.icon.name
            text: appActions.seekForwardSmallAction.text
            onTriggered: appActions.seekForwardSmallAction.trigger()
        }
        Labs.MenuItem {
            icon.name: appActions.seekBackwardSmallAction.icon.name
            text: appActions.seekBackwardSmallAction.text
            onTriggered: appActions.seekBackwardSmallAction.trigger()
        }

        Labs.MenuSeparator {}

        Labs.MenuItem {
            icon.name: appActions.seekForwardMediumAction.icon.name
            text: appActions.seekForwardMediumAction.text
            onTriggered: appActions.seekForwardMediumAction.trigger()
        }

        Labs.MenuItem {
            icon.name: appActions.seekBackwardMediumAction.icon.name
            text: appActions.seekBackwardMediumAction.text
            onTriggered: appActions.seekBackwardMediumAction.trigger()
        }

        Labs.MenuSeparator {}

        Labs.MenuItem {
            icon.name: appActions.seekForwardBigAction.icon.name
            text: appActions.seekForwardBigAction.text
            onTriggered: appActions.seekForwardBigAction.trigger()
        }

        Labs.MenuItem {
            icon.name: appActions.seekBackwardBigAction.icon.name
            text: appActions.seekBackwardBigAction.text
            onTriggered: appActions.seekBackwardBigAction.trigger()
        }

        Labs.MenuSeparator {}

        Labs.MenuItem {
            icon.name: appActions.seekNextSubtitleAction.icon.name
            text: appActions.seekNextSubtitleAction.text
            onTriggered: appActions.seekNextSubtitleAction.trigger()
        }

        Labs.MenuItem {
            icon.name: appActions.seekPrevSubtitleAction.icon.name
            text: appActions.seekPrevSubtitleAction.text
            onTriggered: appActions.seekPrevSubtitleAction.trigger()
        }

        Labs.MenuSeparator {}

        Labs.MenuItem {
            icon.name: appActions.seekNextChapterAction.icon.name
            text: appActions.seekNextChapterAction.text
            onTriggered: appActions.seekNextChapterAction.trigger()
        }

        Labs.MenuItem {
            icon.name: appActions.seekPreviousChapterAction.icon.name
            text: appActions.seekPreviousChapterAction.text
            onTriggered: appActions.seekPreviousChapterAction.trigger()
        }

        Labs.MenuSeparator {}

        Labs.MenuItem {
            icon.name: appActions.frameStepForwardAction.icon.name
            text: appActions.frameStepForwardAction.text
            onTriggered: appActions.frameStepForwardAction.trigger()
        }

        Labs.MenuItem {
            icon.name: appActions.frameStepBackwardAction.icon.name
            text: appActions.frameStepBackwardAction.text
            onTriggered: appActions.frameStepBackwardAction.trigger()
        }

        Labs.MenuItem {
            icon.name: appActions.seekToWatchLaterPositionAction.icon.name
            text: appActions.seekToWatchLaterPositionAction.text
            onTriggered: appActions.seekToWatchLaterPositionAction.trigger()
        }
    }
}
