/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls
import Qt.labs.platform as Labs

import org.kde.haruna

Labs.Menu {
    id: root

    title: i18nc("@title:menu", "&View")

    Labs.MenuItem {
        icon.name: appActions.toggleFullscreenAction.icon.name
        text: appActions.toggleFullscreenAction.text
        onTriggered: appActions.toggleFullscreenAction.trigger()
    }

    Labs.MenuItem {
        icon.name: appActions.toggleMenuBarAction.icon.name
        text: {
            const mainWindow = Window.window as Main
            if (mainWindow?.menuBar.visible) {
                return i18nc("@action:inmenu", "Hide Menubar")
            } else {
                return i18nc("@action:inmenu", "Show Menubar")
            }
        }
        onTriggered: appActions.toggleMenuBarAction.trigger()
    }

    Labs.MenuItem {
        icon.name: appActions.toggleHeaderAction.icon.name
        text: {
            const mainWindow = Window.window as Main
            if (mainWindow?.header.visible) {
                return i18nc("@action:inmenu", "Hide Toolbar")
            } else {
                return i18nc("@action:inmenu", "Show Toolbar")
            }
        }
        onTriggered: appActions.toggleHeaderAction.trigger()
    }
}
