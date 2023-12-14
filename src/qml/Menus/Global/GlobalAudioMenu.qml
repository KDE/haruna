/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQml
import QtQuick
import QtQuick.Controls
import Qt.labs.platform as Labs

import org.kde.haruna

Labs.Menu {
    id: root

    title: i18nc("@title:menu", "&Audio")

    Labs.Menu {
        id: audioMenu

        Repeater {
            model: mpv.audioTracksModel
            delegate: MenuItem {
                checkable: true
                checked: model.id === mpv.audioId
                text: model.text
                onTriggered: {
                    mpv.audioId = model.id
                }
            }
        }
    }

    Labs.MenuSeparator {}

    Labs.MenuItem {
        icon.name: appActions.muteAction.icon.name
        text: appActions.muteAction.text
        onTriggered: appActions.muteAction.trigger()
    }

    Labs.MenuItem {
        icon.name: appActions.volumeUpAction.icon.name
        text: appActions.volumeUpAction.text
        onTriggered: appActions.volumeUpAction.trigger()
    }

    Labs.MenuItem {
        icon.name: appActions.volumeDownAction.icon.name
        text: appActions.volumeDownAction.text
        onTriggered: appActions.volumeDownAction.trigger()
    }

    Labs.MenuItem {
        icon.name: appActions.audioCycleUpAction.icon.name
        text: appActions.audioCycleUpAction.text
        onTriggered: appActions.audioCycleUpAction.trigger()
    }

    Labs.MenuItem {
        icon.name: appActions.audioCycleDownAction.icon.name
        text: appActions.audioCycleDownAction.text
        onTriggered: appActions.audioCycleDownAction.trigger()
    }
}
