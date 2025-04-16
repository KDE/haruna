/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls

import org.kde.haruna

Menu {
    id: root

    required property MpvVideo m_mpv

    MenuItem { action: appActions.playPauseAction }
    MenuItem { action: appActions.playNextAction }
    MenuItem { action: appActions.playPreviousAction }
    Menu {
        title: i18nc("@title:menu", "Playback Speed")
        MenuItem { action: appActions.playbackSpeedIncreaseAction }
        MenuItem { action: appActions.playbackSpeedDecreaseAction }
        MenuItem { action: appActions.playbackSpeedResetAction }
    }

    MenuSeparator {}

    MenuItem { action: appActions.openFileAction }
    MenuItem { action: appActions.openUrlAction }

    MenuSeparator {}

    VideoMenu {}
    AudioMenu {
        m_mpv: root.m_mpv
    }
    SubtitlesMenu {
        m_mpv: root.m_mpv
    }

    MenuSeparator {}

    MenuItem { action: appActions.configureAction }
}
