/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import Qt.labs.platform as Labs

import org.kde.haruna

Labs.Menu {
    id: root

    required property Loader m_settingsLoader

    title: i18nc("@title:menu", "&Settings")

    Labs.MenuItem {
        text: i18nc("@action:inmenu", "General")
        onTriggered: {
            root.m_settingsLoader.openSettingPage(SettingsWindow.Page.General)
        }
    }
    Labs.MenuItem {
        text: i18nc("@action:inmenu", "Playback")
        onTriggered: {
            root.m_settingsLoader.openSettingPage(SettingsWindow.Page.Playback)
        }
    }
    Labs.MenuItem {
        text: i18nc("@action:inmenu", "Video")
        onTriggered: {
            root.m_settingsLoader.openSettingPage(SettingsWindow.Page.Video)
        }
    }
    Labs.MenuItem {
        text: i18nc("@action:inmenu", "Audio")
        onTriggered: {
            root.m_settingsLoader.openSettingPage(SettingsWindow.Page.Audio)
        }
    }
    Labs.MenuItem {
        text: i18nc("@action:inmenu", "Subtitles")
        onTriggered: {
            root.m_settingsLoader.openSettingPage(SettingsWindow.Page.Subtitles)
        }
    }
    Labs.MenuItem {
        text: i18nc("@action:inmenu", "Playlist")
        onTriggered: {
            root.m_settingsLoader.openSettingPage(SettingsWindow.Page.Playlist)
        }
    }
    Labs.MenuItem {
        text: i18nc("@action:inmenu", "Mouse")
        onTriggered: {
            root.m_settingsLoader.openSettingPage(SettingsWindow.Page.Mouse)
        }
    }
    Labs.MenuItem {
        text: i18nc("@action:inmenu", "Shortcuts")
        icon.name: "configure-shortcuts"
        onTriggered: {
            root.m_settingsLoader.openSettingPage(SettingsWindow.Page.Shortcuts)
        }
    }
    Labs.MenuItem {
        text: i18nc("@action:inmenu", "Custom Commands")
        onTriggered: {
            root.m_settingsLoader.openSettingPage(SettingsWindow.Page.CustomCommands)
        }
    }
    Labs.MenuItem {
        text: i18nc("@action:inmenu", "Debug")
        onTriggered: {
            root.m_settingsLoader.openSettingPage(SettingsWindow.Page.Debug)
        }
    }
    Labs.MenuItem {
        text: i18nc("@action:inmenu", "About")
        onTriggered: {
            root.m_settingsLoader.openSettingPage(SettingsWindow.Page.About)
        }
    }
}
