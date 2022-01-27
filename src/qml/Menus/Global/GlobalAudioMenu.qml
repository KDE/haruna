/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQml 2.12
import QtQuick 2.12
import QtQuick.Controls 2.12
import Qt.labs.platform 1.1 as Labs

Labs.Menu {
    id: root

    title: i18n("&Audio")

    Labs.Menu {
        id: audioMenu

        title: i18n("&Primary Track")

        Instantiator {
            id: audioMenuInstantiator
            model: 0
            onObjectAdded: audioMenu.insertItem( index, object )
            onObjectRemoved: audioMenu.removeItem( object )
            delegate: Labs.MenuItem {
                id: audioMenuItem
                checkable: true
                checked: model.id === mpv.audioId
                text: model.text
                onTriggered: mpv.audioId = model.id
            }
        }
        Connections {
            target: mpv
            onFileLoaded: {
                audioMenuInstantiator.model = mpv.audioTracksModel
            }
        }
    }

    Labs.MenuSeparator {}

    Labs.MenuItem {
        icon.name: actions.muteAction.icon.name
        text: actions.muteAction.text
        onTriggered: actions.muteAction.trigger()
    }

    Labs.MenuItem {
        icon.name: actions.volumeUpAction.icon.name
        text: actions.volumeUpAction.text
        onTriggered: actions.volumeUpAction.trigger()
    }

    Labs.MenuItem {
        icon.name: actions.volumeDownAction.icon.name
        text: actions.volumeDownAction.text
        onTriggered: actions.volumeDownAction.trigger()
    }

    Labs.MenuItem {
        icon.name: actions.audioCycleUpAction.icon.name
        text: actions.audioCycleUpAction.text
        onTriggered: actions.audioCycleUpAction.trigger()
    }

    Labs.MenuItem {
        icon.name: actions.audioCycleDownAction.icon.name
        text: actions.audioCycleDownAction.text
        onTriggered: actions.audioCycleDownAction.trigger()
    }
}
