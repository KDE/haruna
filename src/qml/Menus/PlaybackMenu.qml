/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

Menu {
    id: root

    title: i18n("&Playback")

    MenuItem { action: actions.playPauseAction }
    MenuItem { action: actions.playNextAction }
    MenuItem { action: actions.playPreviousAction }

    MenuItem { action: actions.restartPlaybackAction }
    MenuItem { action: actions.setLoopAction }

    MenuSeparator {}

    MenuItem { action: actions.playbackSpeedIncreaseAction }
    MenuItem { action: actions.playbackSpeedDecreaseAction }
    MenuItem { action: actions.playbackSpeedResetAction }

    MenuSeparator {}

    Menu {
        title: i18n("Seek")
        MenuItem { action: actions.seekForwardSmallAction }
        MenuItem { action: actions.seekBackwardSmallAction }

        MenuSeparator {}

        MenuItem { action: actions.seekForwardMediumAction }
        MenuItem { action: actions.seekBackwardMediumAction }

        MenuSeparator {}

        MenuItem { action: actions.seekForwardBigAction }
        MenuItem { action: actions.seekBackwardBigAction }

        MenuSeparator {}

        MenuItem { action: actions.seekNextSubtitleAction }
        MenuItem { action: actions.seekPreviousSubtitleAction }

        MenuSeparator {}

        MenuItem { action: actions.seekPreviousChapterAction }
        MenuItem { action: actions.seekNextChapterAction }

        MenuSeparator {}

        MenuItem { action: actions.frameStepForwardAction }
        MenuItem { action: actions.frameStepBackwardAction }
        MenuItem { action: actions.seekToWatchLaterPositionAction }
    }
}
