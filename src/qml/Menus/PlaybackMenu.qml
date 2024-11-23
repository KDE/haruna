/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick.Controls

Menu {
    id: root

    title: i18nc("@title:menu", "&Playback")

    MenuItem { action: appActions.playPauseAction }
    MenuItem { action: appActions.playNextAction }
    MenuItem { action: appActions.playPreviousAction }

    MenuItem { action: appActions.restartPlaybackAction }
    MenuItem { action: appActions.setLoopAction }

    MenuSeparator {}

    MenuItem { action: appActions.playbackSpeedIncreaseAction }
    MenuItem { action: appActions.playbackSpeedDecreaseAction }
    MenuItem { action: appActions.playbackSpeedResetAction }

    MenuSeparator {}

    Menu {
        title: i18nc("@title:menu", "Seek")
        MenuItem { action: appActions.seekForwardSmallAction }
        MenuItem { action: appActions.seekBackwardSmallAction }

        MenuSeparator {}

        MenuItem { action: appActions.seekForwardMediumAction }
        MenuItem { action: appActions.seekBackwardMediumAction }

        MenuSeparator {}

        MenuItem { action: appActions.seekForwardBigAction }
        MenuItem { action: appActions.seekBackwardBigAction }

        MenuSeparator {}

        MenuItem { action: appActions.seekNextSubtitleAction }
        MenuItem { action: appActions.seekPreviousSubtitleAction }

        MenuSeparator {}

        MenuItem { action: appActions.seekNextChapterAction }
        MenuItem { action: appActions.seekPreviousChapterAction }

        MenuSeparator {}

        MenuItem { action: appActions.frameStepForwardAction }
        MenuItem { action: appActions.frameStepBackwardAction }
        MenuItem { action: appActions.seekToWatchLaterPositionAction }
    }
}
