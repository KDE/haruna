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

    title: i18nc("@title:menu", "&File")

    Labs.MenuItem {
        icon.name: appActions.openFileAction.icon.name
        text: appActions.openFileAction.text
        onTriggered: appActions.openFileAction.trigger()
    }

    Labs.MenuItem {
        icon.name: appActions.openUrlAction.icon.name
        text: appActions.openUrlAction.text
        onTriggered: appActions.openUrlAction.trigger()
    }

    Labs.MenuItem {
        icon.name: appActions.loadLastPlayedFileAction.icon.name
        text: appActions.loadLastPlayedFileAction.text
        onTriggered: appActions.loadLastPlayedFileAction.trigger()
    }

    Labs.Menu {
        id: recentFilesMenu

        title: i18nc("@title:menu", "Recent Files")
        onAboutToShow: recentFilesInstantiator.model = recentFilesModel

        Instantiator {
            id: recentFilesInstantiator

            model: 0
            onObjectAdded: recentFilesMenu.insertItem( index, object )
            onObjectRemoved: recentFilesMenu.removeItem( object )
            delegate: Labs.MenuItem {
                text: model.name
                onTriggered: window.openFile(model.path)
            }
        }
    }


    Labs.MenuSeparator {}

    Labs.MenuItem {
        icon.name: appActions.quitApplicationAction.icon.name
        text: appActions.quitApplicationAction.text
        onTriggered: appActions.quitApplicationAction.trigger()
    }
}
