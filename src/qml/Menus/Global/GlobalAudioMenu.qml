/*
 * SPDX-FileCopyrightText: 2022 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQml
import QtQuick
import Qt.labs.platform as Labs

import org.kde.haruna

Labs.Menu {
    id: root

    required property MpvVideo m_mpv

    title: i18nc("@title:menu", "&Audio")

    Labs.Menu {
        id: audioMenu

        title: i18nc("@title:menu", "&Track")
        enabled: root.m_mpv.audioTracksModel.rowCount() > 0

        Instantiator {
            model: root.m_mpv.audioTracksModel
            delegate: Labs.MenuItem {
                id: delegate

                required property string displayText
                required property string language
                required property string title
                required property string codec
                required property string trackId

                checkable: true
                checked: delegate.trackId === root.m_mpv.audioId
                text: delegate.displayText
                onTriggered: {
                    root.m_mpv.audioId = delegate.trackId
                }
            }
            onObjectAdded: function(index, object) {
                audioMenu.insertItem(index, object)
            }
            onObjectRemoved: function(index, object) {
                audioMenu.removeItem(object)
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
