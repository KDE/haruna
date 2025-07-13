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

    title: i18nc("@title:menu", "&View")

    MenuItem { action: HarunaApp.actions.toggleFullscreenAction }

    MenuItem {
        action: HarunaApp.actions.toggleMenuBarAction
        text: {
            const mainWindow = Window.window as Main
            if (mainWindow?.menuBar.visible) {
                return i18nc("@action:inmenu", "Hide Menubar")
            } else {
                return i18nc("@action:inmenu", "Show Menubar")
            }
        }
    }
    MenuItem {
        action: HarunaApp.actions.toggleHeaderAction
        text: {
            const mainWindow = Window.window as Main
            if (mainWindow?.header.visible) {
                return i18nc("@action:inmenu", "Hide Toolbar")
            } else {
                return i18nc("@action:inmenu", "Show Toolbar")
            }
        }
    }
}
