/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

import org.kde.haruna 1.0

Menu {
    id: root

    MenuItem { action: actions.playPauseAction }
    MenuItem { action: actions.playNextAction }
    MenuItem { action: actions.playPreviousAction }
    Menu {
        title: i18n("Playback speed")
        MenuItem { action: actions.increasePlayBackSpeedAction }
        MenuItem { action: actions.decreasePlayBackSpeedAction }
        MenuItem { action: actions.resetPlayBackSpeedAction }
    }

    MenuSeparator {}

    MenuItem { action: actions.openAction }
    MenuItem { action: actions.openUrlAction }

    MenuSeparator {}

    VideoMenu {}
    AudioMenu {}
    SubtitlesMenu {}

    MenuSeparator {}

    MenuItem { action: actions.configureAction }
}
