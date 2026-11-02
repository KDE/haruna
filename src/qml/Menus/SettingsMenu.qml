/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls

import org.kde.ki18n

Menu {
    id: root

    title: KI18n.i18nc("@title:menu", "&Settings")

    MenuItem {
        action: HarunaApp.actions.configureGeneralAction
        text: KI18n.i18nc("@action:inmenu", "General")
    }
    MenuItem {
        action: HarunaApp.actions.configureOsdAction
        text: KI18n.i18nc("@action:inmenu", "Osd")
    }
    MenuItem {
        action: HarunaApp.actions.configurePlaybackAction
        text: KI18n.i18nc("@action:inmenu", "Playback")
    }
    MenuItem {
        action: HarunaApp.actions.configureVideoAction
        text: KI18n.i18nc("@action:inmenu", "Video")
    }
    MenuItem {
        action: HarunaApp.actions.configureAudioAction
        text: KI18n.i18nc("@action:inmenu", "Audio")
    }
    MenuItem {
        action: HarunaApp.actions.configureSubtitlesAction
        text: KI18n.i18nc("@action:inmenu", "Subtitles")
    }
    MenuItem {
        action: HarunaApp.actions.configurePlaylistAction
        text: KI18n.i18nc("@action:inmenu", "Playlist")
    }
    MenuItem {
        action: HarunaApp.actions.configureMouseAction
        text: KI18n.i18nc("@action:inmenu", "Mouse")
    }
    MenuItem {
        action: HarunaApp.actions.configureShortcutsAction
        text: KI18n.i18nc("@action:inmenu", "Shortcuts")
    }
    MenuItem {
        action: HarunaApp.actions.configureCustomCommandsAction
        text: KI18n.i18nc("@action:inmenu", "Custom Commands")
    }
    MenuItem {
        action: HarunaApp.actions.configureDatabaseAction
        text: KI18n.i18nc("@action:inmenu", "Database")
    }
    MenuItem {
        action: HarunaApp.actions.configureDebugAction
        text: KI18n.i18nc("@action:inmenu", "Debug")
    }
}
