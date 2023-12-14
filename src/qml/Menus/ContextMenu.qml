/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls

Menu {
    id: root

    MenuItem { action: appActions.playPauseAction }
    MenuItem { action: appActions.playNextAction }
    MenuItem { action: appActions.playPreviousAction }
    Menu {
        title: i18nc("@title:menu", "Playback speed")
        MenuItem { action: appActions.playbackSpeedIncreaseAction }
        MenuItem { action: appActions.playbackSpeedDecreaseAction }
        MenuItem { action: appActions.playbackSpeedResetAction }
    }

    MenuSeparator {}

    MenuItem { action: appActions.openFileAction }
    MenuItem { action: appActions.openUrlAction }

    MenuSeparator {}

    VideoMenu {}
    AudioMenu {}
    SubtitlesMenu {}

    MenuSeparator {}

    MenuItem { action: appActions.configureAction }
}
