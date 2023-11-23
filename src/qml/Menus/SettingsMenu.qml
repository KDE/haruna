/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls

import "../Settings"

Menu {
    id: root

    title: i18nc("@title:menu", "&Settings")

    MenuItem { action: appActions.configureShortcutsAction }
    MenuSeparator {}
    MenuItem {
        text: i18nc("@action:inmenu", "General")
        onTriggered: {
            settingsLoader.active = true
            settingsLoader.item.currentPage = SettingsWindow.Page.General
            actionsModel.signalEmitter("configureAction")
        }
    }
    MenuItem {
        text: i18nc("@action:inmenu", "Playback")
        onTriggered: {
            settingsLoader.active = true
            settingsLoader.item.currentPage = SettingsWindow.Page.Playback
            actionsModel.signalEmitter("configureAction")
        }
    }
    MenuItem {
        text: i18nc("@action:inmenu", "Video")
        onTriggered: {
            settingsLoader.active = true
            settingsLoader.item.currentPage = SettingsWindow.Page.Video
            actionsModel.signalEmitter("configureAction")
        }
    }
    MenuItem {
        text: i18nc("@action:inmenu", "Audio")
        onTriggered: {
            settingsLoader.active = true
            settingsLoader.item.currentPage = SettingsWindow.Page.Audio
            actionsModel.signalEmitter("configureAction")
        }
    }
    MenuItem {
        text: i18nc("@action:inmenu", "Subtitles")
        onTriggered: {
            settingsLoader.active = true
            settingsLoader.item.currentPage = SettingsWindow.Page.Subtitles
            actionsModel.signalEmitter("configureAction")
        }
    }
    MenuItem {
        text: i18nc("@action:inmenu", "Playlist")
        onTriggered: {
            settingsLoader.active = true
            settingsLoader.item.currentPage = SettingsWindow.Page.Playlist
            actionsModel.signalEmitter("configureAction")
        }
    }
    MenuItem {
        text: i18nc("@action:inmenu", "Mouse")
        onTriggered: {
            settingsLoader.active = true
            settingsLoader.item.currentPage = SettingsWindow.Page.Mouse
            actionsModel.signalEmitter("configureAction")
        }
    }
    MenuItem {
        text: i18nc("@action:inmenu", "Shortcuts")
        onTriggered: {
            settingsLoader.active = true
            settingsLoader.item.currentPage = SettingsWindow.Page.Shortcuts
            actionsModel.signalEmitter("configureAction")
        }
    }
    MenuItem {
        text: i18nc("@action:inmenu", "Custom Commands")
        onTriggered: {
            settingsLoader.active = true
            settingsLoader.item.currentPage = SettingsWindow.Page.CustomCommands
            actionsModel.signalEmitter("configureAction")
        }
    }
    MenuItem {
        text: i18nc("@action:inmenu", "Debug")
        onTriggered: {
            settingsLoader.active = true
            settingsLoader.item.currentPage = SettingsWindow.Page.Debug
            actionsModel.signalEmitter("configureAction")
        }
    }
    MenuItem {
        text: i18nc("@action:inmenu", "About")
        onTriggered: {
            settingsLoader.active = true
            settingsLoader.item.currentPage = SettingsWindow.Page.About
            actionsModel.signalEmitter("configureAction")
        }
    }
}
