/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQml
import QtQuick
import QtQuick.Controls

Menu {
    id: root

    title: i18nc("@title:menu", "&Audio")

    Menu {
        id: audioMenu

        title: i18nc("@title:menu", "&Primary Track")
        enabled: mpv.audioTracksModel.rowCount() > 0

        Instantiator {
            id: audioMenuInstantiator
            model: mpv.audioTracksModel
            onObjectAdded: (index, object) => audioMenu.insertItem(index, object)
            onObjectRemoved: (index, object) => audioMenu.removeItem(object)
            delegate: MenuItem {
                id: audioMenuItem
                checkable: true
                checked: model.id === mpv.audioId
                text: model.text
                onTriggered: mpv.audioId = model.id
            }
        }
    }

    MenuSeparator {}

    MenuItem { action: appActions.muteAction }
    MenuItem { action: appActions.volumeUpAction }
    MenuItem { action: appActions.volumeDownAction }
    MenuItem { action: appActions.audioCycleUpAction }
    MenuItem { action: appActions.audioCycleDownAction }
}
