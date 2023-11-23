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

    title: i18nc("@title:menu", "&File")

    MenuItem { action: appActions.openFileAction }
    MenuItem { action: appActions.openUrlAction }
    MenuItem { action: appActions.loadLastPlayedFileAction }

    Menu {
        id: recentFilesMenu

        title: i18nc("@title:menu", "Recent Files")

        Instantiator {
            model: recentFilesModel
            delegate: MenuItem {
                text: model.name
                onClicked: {
                    recentFilesMenu.dismiss()
                    window.openFile(model.path)
                }
            }
            onObjectAdded: (index, object) => recentFilesMenu.insertItem(index, object)
            onObjectRemoved: (index, object) => recentFilesMenu.removeItem(object)
        }

        MenuSeparator {}
        MenuItem {
            text: i18nc("@action:inmenu", "Clear List")
            onClicked: recentFilesModel.deleteEntries()
        }
    }


    MenuSeparator {}

    MenuItem { action: appActions.quitApplicationAction }
}
