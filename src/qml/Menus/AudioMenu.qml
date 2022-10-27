/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQml 2.12
import QtQuick 2.12
import QtQuick.Controls 2.12

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
            onObjectAdded: audioMenu.insertItem( index, object )
            onObjectRemoved: audioMenu.removeItem( object )
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
