/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import Qt.labs.platform as Labs

import org.kde.haruna

Labs.Menu {
    id: root

    required property RecentFilesModel m_recentFilesModel

    title: i18nc("@title:menu", "&File")

    Labs.MenuItem {
        icon.name: HarunaApp.actions.openFileAction.icon.name
        text: HarunaApp.actions.openFileAction.text
        onTriggered: HarunaApp.actions.openFileAction.trigger()
    }

    Labs.MenuItem {
        icon.name: HarunaApp.actions.openUrlAction.icon.name
        text: HarunaApp.actions.openUrlAction.text
        onTriggered: HarunaApp.actions.openUrlAction.trigger()
    }

    Labs.MenuItem {
        icon.name: HarunaApp.actions.loadLastPlayedFileAction.icon.name
        text: HarunaApp.actions.loadLastPlayedFileAction.text
        onTriggered: HarunaApp.actions.loadLastPlayedFileAction.trigger()
    }

    Labs.Menu {
        id: recentFilesMenu

        title: i18nc("@title:menu", "Recent Files")
        onAboutToShow: recentFilesInstantiator.model = root.m_recentFilesModel

        Instantiator {
            id: recentFilesInstantiator

            model: 0
            onObjectAdded: (index, object) => recentFilesMenu.insertItem( index, object )
            onObjectRemoved: (index, object) => recentFilesMenu.removeItem( object )
            delegate: Labs.MenuItem {
                id: delegate

                required property string url
                required property string filename
                required property int openedFrom

                text: openedFrom === RecentFilesModel.OpenedFrom.OpenAction
                      || openedFrom === RecentFilesModel.OpenedFrom.ExternalApp
                      ? `<strong>${delegate.filename}</strong>`
                      : delegate.filename
                onTriggered: {
                    recentFilesMenu.close()
                    const mainWindow = Window.window as Main
                    mainWindow.openFile(delegate.url, RecentFilesModel.OpenedFrom.Other)
                }
            }
        }
        Labs.MenuSeparator {}
        Labs.MenuItem {
            text: i18nc("@action:inmenu", "Clear List")
            onTriggered: root.m_recentFilesModel.deleteEntries()
        }
    }


    Labs.MenuSeparator {}

    Labs.MenuItem {
        icon.name: HarunaApp.actions.quitApplicationAction.icon.name
        text: HarunaApp.actions.quitApplicationAction.text
        onTriggered: HarunaApp.actions.quitApplicationAction.trigger()
    }
}
