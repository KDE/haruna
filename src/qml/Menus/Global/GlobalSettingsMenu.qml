/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import Qt.labs.platform as Labs

import org.kde.ki18n

Labs.Menu {
    id: root

    title: KI18n.i18nc("@title:menu", "&Settings")

    Labs.MenuItem {
        text: KI18n.i18nc("@action:inmenu", "General")
        onTriggered: {
            HarunaApp.actions.configureGeneralAction.trigger()
        }
    }
    Labs.MenuItem {
        text: KI18n.i18nc("@action:inmenu", "Osd")
        onTriggered: {
            HarunaApp.actions.configureOsdAction.trigger()
        }
    }
    Labs.MenuItem {
        text: KI18n.i18nc("@action:inmenu", "Playback")
        onTriggered: {
            HarunaApp.actions.configurePlaybackAction.trigger()
        }
    }
    Labs.MenuItem {
        text: KI18n.i18nc("@action:inmenu", "Video")
        onTriggered: {
            HarunaApp.actions.configureVideoAction.trigger()
        }
    }
    Labs.MenuItem {
        text: KI18n.i18nc("@action:inmenu", "Audio")
        onTriggered: {
            HarunaApp.actions.configureAudioAction.trigger()
        }
    }
    Labs.MenuItem {
        text: KI18n.i18nc("@action:inmenu", "Subtitles")
        onTriggered: {
            HarunaApp.actions.configureSubtitlesAction.trigger()
        }
    }
    Labs.MenuItem {
        text: KI18n.i18nc("@action:inmenu", "Playlist")
        onTriggered: {
            HarunaApp.actions.configurePlaylistAction.trigger()
        }
    }
    Labs.MenuItem {
        text: KI18n.i18nc("@action:inmenu", "Mouse")
        onTriggered: {
            HarunaApp.actions.configureMouseAction.trigger()
        }
    }
    Labs.MenuItem {
        text: KI18n.i18nc("@action:inmenu", "Shortcuts")
        icon.name: "configure-shortcuts"
        onTriggered: {
            HarunaApp.actions.configureShortcutsAction.trigger()
        }
    }
    Labs.MenuItem {
        text: KI18n.i18nc("@action:inmenu", "Custom Commands")
        onTriggered: {
            HarunaApp.actions.configureCustomCommandsAction.trigger()
        }
    }
    Labs.MenuItem {
        text: KI18n.i18nc("@action:inmenu", "Database")
        onTriggered: {
            HarunaApp.actions.configureDatabaseAction.trigger()
        }
    }
    Labs.MenuItem {
        text: KI18n.i18nc("@action:inmenu", "Debug")
        onTriggered: {
            HarunaApp.actions.configureDebugAction.trigger()
        }
    }
}
