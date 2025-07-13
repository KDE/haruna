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
        enabled: root.m_mpv.audioTracksModel.rowCount > 0

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
        icon.name: HarunaApp.actions.muteAction.icon.name
        text: HarunaApp.actions.muteAction.text
        onTriggered: HarunaApp.actions.muteAction.trigger()
    }

    Labs.MenuItem {
        icon.name: HarunaApp.actions.volumeUpAction.icon.name
        text: HarunaApp.actions.volumeUpAction.text
        onTriggered: HarunaApp.actions.volumeUpAction.trigger()
    }

    Labs.MenuItem {
        icon.name: HarunaApp.actions.volumeDownAction.icon.name
        text: HarunaApp.actions.volumeDownAction.text
        onTriggered: HarunaApp.actions.volumeDownAction.trigger()
    }

    Labs.MenuItem {
        icon.name: HarunaApp.actions.audioCycleUpAction.icon.name
        text: HarunaApp.actions.audioCycleUpAction.text
        onTriggered: HarunaApp.actions.audioCycleUpAction.trigger()
    }

    Labs.MenuItem {
        icon.name: HarunaApp.actions.audioCycleDownAction.icon.name
        text: HarunaApp.actions.audioCycleDownAction.text
        onTriggered: HarunaApp.actions.audioCycleDownAction.trigger()
    }
}
