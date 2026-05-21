/*
 * SPDX-FileCopyrightText: 2026 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma Singleton

import QtQuick

import org.kde.haruna

QtObject {
    id: root

    property RecentFilesModel recentFilesModel: RecentFilesModel {}
    property ActionsModel actionsModel: ActionsModel {}
    property ProxyActionsModel proxyActionsModel: ProxyActionsModel {
        sourceModel: root.actionsModel
    }
    property MouseActionsModel mouseActionsModel: MouseActionsModel {}
    property CustomCommandsModel customCommandsModel: CustomCommandsModel {
        appActionsModel: root.actionsModel
        Component.onCompleted: init()
    }

}
