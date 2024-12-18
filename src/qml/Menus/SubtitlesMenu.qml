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

    required property MpvVideo m_mpv

    title: i18nc("@title:menu", "&Subtitles")

    SubtitleTracksMenu {
        m_mpv: root.m_mpv
        isPrimarySubtitleMenu: true
        title: i18nc("@title:menu", "Primary Subtitle")
    }

    SubtitleTracksMenu {
        m_mpv: root.m_mpv
        isPrimarySubtitleMenu: false
        title: i18nc("@title:menu", "Secondary Subtitle")
    }

    MenuSeparator {}

    MenuItem {
        action: appActions.openSubtitlesFileAction
        enabled: root.m_mpv.currentUrl.toString() !== ""
    }
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
