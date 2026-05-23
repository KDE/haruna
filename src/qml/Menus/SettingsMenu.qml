/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls

import org.kde.ki18n

import org.kde.haruna

Menu {
    id: root

    required property SettingsWindow m_settingsLoader

    title: KI18n.i18nc("@title:menu", "&Settings")

    MenuItem {
        text: KI18n.i18nc("@action:inmenu", "General")
        onTriggered: {
            root.m_settingsLoader.openSettingPage(SettingsWindow.Page.General)
        }
    }
    MenuItem {
        text: KI18n.i18nc("@action:inmenu", "Playback")
        onTriggered: {
            root.m_settingsLoader.openSettingPage(SettingsWindow.Page.Playback)
        }
    }
    MenuItem {
        text: KI18n.i18nc("@action:inmenu", "Video")
        onTriggered: {
            root.m_settingsLoader.openSettingPage(SettingsWindow.Page.Video)
        }
    }
    MenuItem {
        text: KI18n.i18nc("@action:inmenu", "Audio")
        onTriggered: {
            root.m_settingsLoader.openSettingPage(SettingsWindow.Page.Audio)
        }
    }
    MenuItem {
        text: KI18n.i18nc("@action:inmenu", "Subtitles")
        onTriggered: {
            root.m_settingsLoader.openSettingPage(SettingsWindow.Page.Subtitles)
        }
    }
    MenuItem {
        text: KI18n.i18nc("@action:inmenu", "Playlist")
        onTriggered: {
            root.m_settingsLoader.openSettingPage(SettingsWindow.Page.Playlist)
        }
    }
    MenuItem {
        text: KI18n.i18nc("@action:inmenu", "Mouse")
        onTriggered: {
            root.m_settingsLoader.openSettingPage(SettingsWindow.Page.Mouse)
        }
    }
    MenuItem {
        text: KI18n.i18nc("@action:inmenu", "Shortcuts")
        icon.name: "configure-shortcuts"
        onTriggered: {
            root.m_settingsLoader.openSettingPage(SettingsWindow.Page.Shortcuts)
        }
    }
    MenuItem {
        text: KI18n.i18nc("@action:inmenu", "Custom Commands")
        onTriggered: {
            root.m_settingsLoader.openSettingPage(SettingsWindow.Page.CustomCommands)
        }
    }
    MenuItem {
        text: KI18n.i18nc("@action:inmenu", "Debug")
        onTriggered: {
            root.m_settingsLoader.openSettingPage(SettingsWindow.Page.Debug)
        }
    }
    MenuItem {
        text: KI18n.i18nc("@action:inmenu", "About")
        onTriggered: {
            root.m_settingsLoader.openSettingPage(SettingsWindow.Page.About)
        }
    }
}
