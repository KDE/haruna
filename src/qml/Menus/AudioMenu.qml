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

    title: i18nc("@title:menu", "&Audio")

    AudioTracksMenu {
        id: audioMenu

        m_mpv: root.m_mpv

        y: parent.height
        title: i18nc("@title:menu", "&Track")
    }

    MenuSeparator {}

    MenuItem { action: HarunaApp.actions.muteAction }
    MenuItem { action: HarunaApp.actions.volumeUpAction }
    MenuItem { action: HarunaApp.actions.volumeDownAction }
    MenuItem { action: HarunaApp.actions.audioCycleUpAction }
    MenuItem { action: HarunaApp.actions.audioCycleDownAction }
}
