/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

import org.kde.haruna
import org.kde.haruna.settings

Menu {
    id: root

    required property RecentFilesModel m_recentFilesModel

    title: i18nc("@title:menu", "&File")

    MenuItem { action: appActions.openFileAction }
    MenuItem { action: appActions.openUrlAction }
    MenuItem {
        action: appActions.loadLastPlayedFileAction
        enabled: GeneralSettings.lastPlayedFile !== ""
        Component.onCompleted: console.log(GeneralSettings.lastPlayedFile)
    }

    Menu {
        id: recentFilesMenu

        title: i18nc("@title:menu", "Recent Files")

        Instantiator {
            model: root.m_recentFilesModel
            delegate: MenuItem {
                id: delegate

                required property string name
                required property string path

                text: delegate.name

                onClicked: {
                    recentFilesMenu.dismiss()
                    const mainWindow = Window.window as Main
                    mainWindow.openFile(delegate.path)
                }
            }
            onObjectAdded: (index, object) => recentFilesMenu.insertItem(index, object)
            onObjectRemoved: (index, object) => recentFilesMenu.removeItem(object)
        }

        MenuSeparator {}
        MenuItem {
            text: i18nc("@action:inmenu", "Clear List")
            onClicked: root.m_recentFilesModel.deleteEntries()
        }
    }


    MenuSeparator {}

    MenuItem { action: appActions.quitApplicationAction }
}
