/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick.Controls

import org.kde.haruna

Menu {
    id: root

    title: i18nc("@title:menu", "&Playback")

    MenuItem { action: HarunaApp.actions.playPauseAction }
    MenuItem { action: HarunaApp.actions.playNextAction }
    MenuItem { action: HarunaApp.actions.playPreviousAction }

    MenuItem { action: HarunaApp.actions.restartPlaybackAction }
    MenuItem { action: HarunaApp.actions.setLoopAction }

    MenuSeparator {}

    MenuItem { action: HarunaApp.actions.playbackSpeedIncreaseAction }
    MenuItem { action: HarunaApp.actions.playbackSpeedDecreaseAction }
    MenuItem { action: HarunaApp.actions.playbackSpeedResetAction }

    MenuSeparator {}

    Menu {
        title: i18nc("@title:menu", "Seek")
        MenuItem { action: HarunaApp.actions.seekForwardSmallAction }
        MenuItem { action: HarunaApp.actions.seekBackwardSmallAction }

        MenuSeparator {}

        MenuItem { action: HarunaApp.actions.seekForwardMediumAction }
        MenuItem { action: HarunaApp.actions.seekBackwardMediumAction }

        MenuSeparator {}

        MenuItem { action: HarunaApp.actions.seekForwardBigAction }
        MenuItem { action: HarunaApp.actions.seekBackwardBigAction }

        MenuSeparator {}

        MenuItem { action: HarunaApp.actions.seekNextSubtitleAction }
        MenuItem { action: HarunaApp.actions.seekPreviousSubtitleAction }

        MenuSeparator {}

        MenuItem { action: HarunaApp.actions.seekNextChapterAction }
        MenuItem { action: HarunaApp.actions.seekPreviousChapterAction }

        MenuSeparator {}

        MenuItem { action: HarunaApp.actions.frameStepForwardAction }
        MenuItem { action: HarunaApp.actions.frameStepBackwardAction }
        MenuItem { action: HarunaApp.actions.seekToWatchLaterPositionAction }
    }
}
