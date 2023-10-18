/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt.labs.platform 1.1 as Labs

import "../../Settings"

Labs.Menu {
    id: root

    title: i18nc("@title:menu", "&Settings")

    Labs.MenuItem {
        icon.name: appActions.configureShortcutsAction.icon.name
        text: appActions.configureShortcutsAction.text
        onTriggered: appActions.configureShortcutsAction.trigger()
    }
    Labs.MenuSeparator {}
    Labs.MenuItem {
        text: i18nc("@action:inmenu", "General")
        onTriggered: {
            settingsLoader.active = true
            settingsLoader.item.currentPage = SettingsWindow.Page.General
            actionsModel.signalEmitter("configureAction")
        }
    }
    Labs.MenuItem {
        text: i18nc("@action:inmenu", "Playback")
        onTriggered: {
            settingsLoader.active = true
            settingsLoader.item.currentPage = SettingsWindow.Page.Playback
            actionsModel.signalEmitter("configureAction")
        }
    }
    Labs.MenuItem {
        text: i18nc("@action:inmenu", "Video")
        onTriggered: {
            settingsLoader.active = true
            settingsLoader.item.currentPage = SettingsWindow.Page.Video
            actionsModel.signalEmitter("configureAction")
        }
    }
    Labs.MenuItem {
        text: i18nc("@action:inmenu", "Audio")
        onTriggered: {
            settingsLoader.active = true
            settingsLoader.item.currentPage = SettingsWindow.Page.Audio
            actionsModel.signalEmitter("configureAction")
        }
    }
    Labs.MenuItem {
        text: i18nc("@action:inmenu", "Subtitles")
        onTriggered: {
            settingsLoader.active = true
            settingsLoader.item.currentPage = SettingsWindow.Page.Subtitles
            actionsModel.signalEmitter("configureAction")
        }
    }
    Labs.MenuItem {
        text: i18nc("@action:inmenu", "Playlist")
        onTriggered: {
            settingsLoader.active = true
            settingsLoader.item.currentPage = SettingsWindow.Page.Playlist
            actionsModel.signalEmitter("configureAction")
        }
    }
    Labs.MenuItem {
        text: i18nc("@action:inmenu", "Mouse")
        onTriggered: {
            settingsLoader.active = true
            settingsLoader.item.currentPage = SettingsWindow.Page.Mouse
            actionsModel.signalEmitter("configureAction")
        }
    }
    Labs.MenuItem {
        text: i18nc("@action:inmenu", "Shortcuts")
        onTriggered: {
            settingsLoader.active = true
            settingsLoader.item.currentPage = SettingsWindow.Page.Shortcuts
            actionsModel.signalEmitter("configureAction")
        }
    }
    Labs.MenuItem {
        text: i18nc("@action:inmenu", "Custom Commands")
        onTriggered: {
            settingsLoader.active = true
            settingsLoader.item.currentPage = SettingsWindow.Page.CustomCommands
            actionsModel.signalEmitter("configureAction")
        }
    }
    Labs.MenuItem {
        text: i18nc("@action:inmenu", "Debug")
        onTriggered: {
            settingsLoader.active = true
            settingsLoader.item.currentPage = SettingsWindow.Page.Debug
            actionsModel.signalEmitter("configureAction")
        }
    }
    Labs.MenuItem {
        text: i18nc("@action:inmenu", "About")
        onTriggered: {
            settingsLoader.active = true
            settingsLoader.item.currentPage = SettingsWindow.Page.About
            actionsModel.signalEmitter("configureAction")
        }
    }
}
