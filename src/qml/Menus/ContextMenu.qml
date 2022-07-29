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

    MenuItem { action: appActions.playPauseAction }
    MenuItem { action: appActions.playNextAction }
    MenuItem { action: appActions.playPreviousAction }
    Menu {
        title: i18n("Playback speed")
        MenuItem { action: appActions.increasePlayBackSpeedAction }
        MenuItem { action: appActions.decreasePlayBackSpeedAction }
        MenuItem { action: appActions.resetPlayBackSpeedAction }
    }

    MenuSeparator {}

    MenuItem { action: appActions.openAction }
    MenuItem { action: appActions.openUrlAction }

    MenuSeparator {}

    VideoMenu {}
    AudioMenu {}
    SubtitlesMenu {}

    MenuSeparator {}

    MenuItem { action: appActions.configureAction }
}
