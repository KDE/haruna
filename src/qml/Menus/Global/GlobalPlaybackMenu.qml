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
        icon.name: actions.playPauseAction.icon.name
        text: actions.playPauseAction.text
        onTriggered: actions.playPauseAction.trigger()
    }

    Labs.MenuItem {
        icon.name: actions.playNextAction.icon.name
        text: actions.playNextAction.text
        onTriggered: actions.playNextAction.trigger()
    }

    Labs.MenuItem {
        icon.name: actions.playPreviousAction.icon.name
        text: actions.playPreviousAction.text
        onTriggered: actions.playPreviousAction.trigger()
    }

    Labs.MenuItem {
        icon.name: actions.setLoopAction.icon.name
        text: actions.setLoopAction.text
        onTriggered: actions.setLoopAction.trigger()
    }

    Labs.MenuSeparator {}

    Labs.MenuItem {
        icon.name: actions.increasePlayBackSpeedAction.icon.name
        text: actions.increasePlayBackSpeedAction.text
        onTriggered: actions.increasePlayBackSpeedAction.trigger()
    }

    Labs.MenuItem {
        icon.name: actions.decreasePlayBackSpeedAction.icon.name
        text: actions.decreasePlayBackSpeedAction.text
        onTriggered: actions.decreasePlayBackSpeedAction.trigger()
    }

    Labs.MenuItem {
        icon.name: actions.resetPlayBackSpeedAction.icon.name
        text: actions.resetPlayBackSpeedAction.text
        onTriggered: actions.resetPlayBackSpeedAction.trigger()
    }

    Labs.MenuSeparator {}

    Labs.Menu {
        title: i18n("Seek")
        Labs.MenuItem {
            icon.name: actions.seekForwardSmallAction.icon.name
            text: actions.seekForwardSmallAction.text
            onTriggered: actions.seekForwardSmallAction.trigger()
        }
        Labs.MenuItem {
            icon.name: actions.seekBackwardSmallAction.icon.name
            text: actions.seekBackwardSmallAction.text
            onTriggered: actions.seekBackwardSmallAction.trigger()
        }

        Labs.MenuSeparator {}

        Labs.MenuItem {
            icon.name: actions.seekForwardMediumAction.icon.name
            text: actions.seekForwardMediumAction.text
            onTriggered: actions.seekForwardMediumAction.trigger()
        }

        Labs.MenuItem {
            icon.name: actions.seekBackwardMediumAction.icon.name
            text: actions.seekBackwardMediumAction.text
            onTriggered: actions.seekBackwardMediumAction.trigger()
        }

        Labs.MenuSeparator {}

        Labs.MenuItem {
            icon.name: actions.seekForwardBigAction.icon.name
            text: actions.seekForwardBigAction.text
            onTriggered: actions.seekForwardBigAction.trigger()
        }

        Labs.MenuItem {
            icon.name: actions.seekBackwardBigAction.icon.name
            text: actions.seekBackwardBigAction.text
            onTriggered: actions.seekBackwardBigAction.trigger()
        }

        Labs.MenuSeparator {}

        Labs.MenuItem {
            icon.name: actions.seekNextSubtitleAction.icon.name
            text: actions.seekNextSubtitleAction.text
            onTriggered: actions.seekNextSubtitleAction.trigger()
        }

        Labs.MenuItem {
            icon.name: actions.seekPrevSubtitleAction.icon.name
            text: actions.seekPrevSubtitleAction.text
            onTriggered: actions.seekPrevSubtitleAction.trigger()
        }

        Labs.MenuSeparator {}

        Labs.MenuItem {
            icon.name: actions.seekPreviousChapterAction.icon.name
            text: actions.seekPreviousChapterAction.text
            onTriggered: actions.seekPreviousChapterAction.trigger()
        }

        Labs.MenuItem {
            icon.name: actions.seekNextChapterAction.icon.name
            text: actions.seekNextChapterAction.text
            onTriggered: actions.seekNextChapterAction.trigger()
        }

        Labs.MenuSeparator {}

        Labs.MenuItem {
            icon.name: actions.frameStepAction.icon.name
            text: actions.frameStepAction.text
            onTriggered: actions.frameStepAction.trigger()
        }

        Labs.MenuItem {
            icon.name: actions.frameBackStepAction.icon.name
            text: actions.frameBackStepAction.text
            onTriggered: actions.frameBackStepAction.trigger()
        }
    }
}
