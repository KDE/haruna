/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

import org.kde.haruna 1.0

Menu {
    id: root

    title: i18n("&File")

    MenuItem { action: actions.openAction }
    MenuItem { action: actions.openUrlAction }

    Menu {
        id: recentFilesMenu

        title: i18n("Recent Files")

        Instantiator {
            model: recentFilesModel
            delegate: MenuItem {
                text: model.name
                onClicked: window.openFile(model.path, true, PlaylistSettings.loadSiblings)
            }
            onObjectAdded: recentFilesMenu.insertItem(index, object)
            onObjectRemoved: recentFilesMenu.removeItem(object)
        }

        MenuSeparator {}
        MenuItem {
            text: i18n("Clear List")
            onClicked: recentFilesModel.clear()
        }
    }


    MenuSeparator {}

    MenuItem { action: actions.quitApplicationAction }
}
