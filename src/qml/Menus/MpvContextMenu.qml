/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

import org.kde.haruna

Loader {
    id: root

    required property MpvVideo m_mpv

    function open() : void {
        if (!root.active) {
            root.active = true
            root.loaded.connect(function() {
                root.open()
            })
            return
        }

        const contextMenu = root.item as Menu
        contextMenu.popup()
    }

    function close() : void {
        root.active = false
    }

    active: false
    asynchronous: true
    sourceComponent: Menu {

        onClosed: root.active = false

        MenuItem { action: HarunaApp.actions.playPauseAction }
        MenuItem { action: HarunaApp.actions.playNextAction }
        MenuItem { action: HarunaApp.actions.playPreviousAction }
        Menu {
            title: i18nc("@title:menu", "Playback Speed")
            MenuItem { action: HarunaApp.actions.playbackSpeedIncreaseAction }
            MenuItem { action: HarunaApp.actions.playbackSpeedDecreaseAction }
            MenuItem { action: HarunaApp.actions.playbackSpeedResetAction }
        }

        MenuSeparator {}

        MenuItem { action: HarunaApp.actions.openFileAction }
        MenuItem { action: HarunaApp.actions.openUrlAction }

        MenuSeparator {}

        VideoMenu {}
        AudioMenu {
            m_mpv: root.m_mpv
        }
        SubtitlesMenu {
            m_mpv: root.m_mpv
        }

        MenuSeparator {}

        MenuItem { action: HarunaApp.actions.configureAction }
    }
}