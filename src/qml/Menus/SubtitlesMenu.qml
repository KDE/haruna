/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQml
import QtQuick
import QtQuick.Controls

import org.kde.haruna

Menu {
    id: root

    title: i18nc("@title:menu", "&Subtitles")

    SubtitleTracksMenu {
        model: mpv.subtitleTracksModel
        isPrimarySubtitleMenu: true
        title: i18nc("@title:menu", "Primary Subtitle")
        enabled: mpv.subtitleTracksModel.rowCount() > 1
    }

    SubtitleTracksMenu {
        model: mpv.subtitleTracksModel
        isPrimarySubtitleMenu: false
        title: i18nc("@title:menu", "Secondary Subtitle")
        enabled: mpv.subtitleTracksModel.rowCount() > 1
    }

    MenuSeparator {}

    MenuItem { action: appActions.openSubtitlesFileAction }
    MenuItem { action: appActions.subtitleIncreaseFontSizeAction }
    MenuItem { action: appActions.subtitleDecreaseFontSizeAction }
    MenuItem { action: appActions.subtitleMoveUpAction }
    MenuItem { action: appActions.subtitleMoveDownAction }
    MenuItem { action: appActions.subtitleQuickenAction }
    MenuItem { action: appActions.subtitleDelayAction }
    MenuItem { action: appActions.subtitleToggleAction }
    MenuItem { action: appActions.subtitleCycleUpAction }
    MenuItem { action: appActions.subtitleCycleDownAction }
}
