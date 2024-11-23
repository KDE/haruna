/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls

import org.kde.haruna

Menu {
    id: root

    required property Loader m_settingsLoader

    title: i18nc("@title:menu", "&Settings")

    MenuItem {
        text: i18nc("@action:inmenu", "General")
        onTriggered: {
            root.m_settingsLoader.active = true
            root.m_settingsLoader.item.currentPage = SettingsWindow.Page.General
            actionsModel.signalEmitter("configureAction")
        }
    }
    MenuItem {
        text: i18nc("@action:inmenu", "Playback")
        onTriggered: {
            root.m_settingsLoader.active = true
            root.m_settingsLoader.item.currentPage = SettingsWindow.Page.Playback
            actionsModel.signalEmitter("configureAction")
        }
    }
    MenuItem {
        text: i18nc("@action:inmenu", "Video")
        onTriggered: {
            root.m_settingsLoader.active = true
            root.m_settingsLoader.item.currentPage = SettingsWindow.Page.Video
            actionsModel.signalEmitter("configureAction")
        }
    }
    MenuItem {
        text: i18nc("@action:inmenu", "Audio")
        onTriggered: {
            root.m_settingsLoader.active = true
            root.m_settingsLoader.item.currentPage = SettingsWindow.Page.Audio
            actionsModel.signalEmitter("configureAction")
        }
    }
    MenuItem {
        text: i18nc("@action:inmenu", "Subtitles")
        onTriggered: {
            root.m_settingsLoader.active = true
            root.m_settingsLoader.item.currentPage = SettingsWindow.Page.Subtitles
            actionsModel.signalEmitter("configureAction")
        }
    }
    MenuItem {
        text: i18nc("@action:inmenu", "Playlist")
        onTriggered: {
            root.m_settingsLoader.active = true
            root.m_settingsLoader.item.currentPage = SettingsWindow.Page.Playlist
            actionsModel.signalEmitter("configureAction")
        }
    }
    MenuItem {
        text: i18nc("@action:inmenu", "Mouse")
        onTriggered: {
            root.m_settingsLoader.active = true
            root.m_settingsLoader.item.currentPage = SettingsWindow.Page.Mouse
            actionsModel.signalEmitter("configureAction")
        }
    }
    MenuItem {
        text: i18nc("@action:inmenu", "Shortcuts")
        icon.name: "configure-shortcuts"
        onTriggered: {
            root.m_settingsLoader.active = true
            root.m_settingsLoader.item.currentPage = SettingsWindow.Page.Shortcuts
            actionsModel.signalEmitter("configureAction")
        }
    }
    MenuItem {
        text: i18nc("@action:inmenu", "Custom Commands")
        onTriggered: {
            root.m_settingsLoader.active = true
            root.m_settingsLoader.item.currentPage = SettingsWindow.Page.CustomCommands
            actionsModel.signalEmitter("configureAction")
        }
    }
    MenuItem {
        text: i18nc("@action:inmenu", "Debug")
        onTriggered: {
            root.m_settingsLoader.active = true
            root.m_settingsLoader.item.currentPage = SettingsWindow.Page.Debug
            actionsModel.signalEmitter("configureAction")
        }
    }
    MenuItem {
        text: i18nc("@action:inmenu", "About")
        onTriggered: {
            root.m_settingsLoader.active = true
            root.m_settingsLoader.item.currentPage = SettingsWindow.Page.About
            actionsModel.signalEmitter("configureAction")
        }
    }
}
