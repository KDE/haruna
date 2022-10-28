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
                onClicked: window.openFile(model.path)
            }
            onObjectAdded: recentFilesMenu.insertItem(index, object)
            onObjectRemoved: recentFilesMenu.removeItem(object)
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
