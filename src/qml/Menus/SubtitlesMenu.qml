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
        action: HarunaApp.actions.addExternalSubtitleAction
        enabled: root.m_mpv.currentUrl.toString() !== ""
    }
    MenuItem { action: HarunaApp.actions.subtitleIncreaseFontSizeAction }
    MenuItem { action: HarunaApp.actions.subtitleDecreaseFontSizeAction }
    MenuItem { action: HarunaApp.actions.subtitleMoveUpAction }
    MenuItem { action: HarunaApp.actions.subtitleMoveDownAction }
    MenuItem { action: HarunaApp.actions.subtitleQuickenAction }
    MenuItem { action: HarunaApp.actions.subtitleDelayAction }
    MenuItem { action: HarunaApp.actions.subtitleToggleAction }
    MenuItem { action: HarunaApp.actions.subtitleCycleUpAction }
    MenuItem { action: HarunaApp.actions.subtitleCycleDownAction }
}
