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

    title: qsTr("&Audio")

    Menu {
        id: audioMenu

        title: qsTr("&Primary Track")

        Instantiator {
            id: audioMenuInstantiator
            model: 0
            onObjectAdded: function(index, object) {
                audioMenu.insertItem( index, object )
            }
            onObjectRemoved: function(index, object) {
                audioMenu.removeItem( object )
            }
            delegate: MenuItem {
                id: audioMenuItem
                checkable: true
                checked: model.id === mpv.audioId
                text: model.text
                onTriggered: mpv.audioId = model.id
            }
        }
        Connections {
            target: mpv
            function onFileLoaded() {
                audioMenuInstantiator.model = mpv.audioTracksModel()
            }
        }
    }

    MenuSeparator {}

    MenuItem { action: actions["muteAction"] }
    MenuItem { action: actions["volumeUpAction"] }
    MenuItem { action: actions["volumeDownAction"] }
}
