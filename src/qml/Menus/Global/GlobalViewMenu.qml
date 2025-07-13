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

    title: i18nc("@title:menu", "&View")

    Labs.MenuItem {
        icon.name: HarunaApp.actions.toggleFullscreenAction.icon.name
        text: HarunaApp.actions.toggleFullscreenAction.text
        onTriggered: HarunaApp.actions.toggleFullscreenAction.trigger()
    }

    Labs.MenuItem {
        icon.name: HarunaApp.actions.toggleMenuBarAction.icon.name
        text: {
            const mainWindow = Window.window as Main
            if (mainWindow?.menuBar.visible) {
                return i18nc("@action:inmenu", "Hide Menubar")
            } else {
                return i18nc("@action:inmenu", "Show Menubar")
            }
        }
        onTriggered: HarunaApp.actions.toggleMenuBarAction.trigger()
    }

    Labs.MenuItem {
        icon.name: HarunaApp.actions.toggleHeaderAction.icon.name
        text: {
            const mainWindow = Window.window as Main
            if (mainWindow?.header.visible) {
                return i18nc("@action:inmenu", "Hide Toolbar")
            } else {
                return i18nc("@action:inmenu", "Show Toolbar")
            }
        }
        onTriggered: HarunaApp.actions.toggleHeaderAction.trigger()
    }
}
