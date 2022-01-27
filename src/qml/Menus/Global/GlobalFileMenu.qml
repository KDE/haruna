/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.12
import Qt.labs.platform 1.1 as Labs

import org.kde.haruna 1.0

Labs.Menu {
    id: root

    title: i18n("&File")

    Labs.MenuItem {
        icon.name: actions.openAction.icon.name
        text: actions.openAction.text
        onTriggered: actions.openAction.trigger()
    }

    Labs.MenuItem {
        icon.name: actions.openUrlAction.icon.name
        text: actions.openUrlAction.text
        onTriggered: actions.openUrlAction.trigger()
    }

    Labs.Menu {
        id: recentFilesMenu

        title: i18n("Recent Files")
        onAboutToShow: recentFilesInstantiator.model = recentFilesModel

        Instantiator {
            id: recentFilesInstantiator

            model: 0
            onObjectAdded: recentFilesMenu.insertItem( index, object )
            onObjectRemoved: recentFilesMenu.removeItem( object )
            delegate: Labs.MenuItem {
                text: model.name
                onTriggered: window.openFile(model.path, true, PlaylistSettings.loadSiblings)
            }
        }
    }


    Labs.MenuSeparator {}

    Labs.MenuItem {
        icon.name: actions.quitApplicationAction.icon.name
        text: actions.quitApplicationAction.text
        onTriggered: actions.quitApplicationAction.trigger()
    }
}
