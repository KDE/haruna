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

    title: qsTr("&Subtitles")

    Menu {
        id: primarySubtitleMenu

        title: qsTr("Primary Subtitle")
        onOpened: primarySubtitleMenuInstantiator.model = mpv.subtitleTracksModel()

        Instantiator {
            id: primarySubtitleMenuInstantiator
            model: 0
            onObjectAdded: function(index, object) {
                primarySubtitleMenu.insertItem( index, object )
            }
            onObjectRemoved: function(index, object) {
                primarySubtitleMenu.removeItem( object )
            }
            delegate: MenuItem {
                enabled: model.id !== mpv.secondarySubtitleId || model.id === 0
                checkable: true
                checked: model.id === mpv.subtitleId
                text: model.text
                onTriggered: mpv.subtitleId = model.id
            }
        }
    }

    Menu {
        id: secondarySubtitleMenu

        title: qsTr("Secondary Subtitle")
        onOpened: secondarySubtitleMenuInstantiator.model = mpv.subtitleTracksModel()

        Instantiator {
            id: secondarySubtitleMenuInstantiator
            model: 0
            onObjectAdded: function(index, object) {
                secondarySubtitleMenu.insertItem( index, object )
            }
            onObjectRemoved: function(index, object) {
                secondarySubtitleMenu.removeItem( object )
            }
            delegate: MenuItem {
                enabled: model.id !== mpv.subtitleId || model.id === 0
                checkable: true
                checked: model.id === mpv.secondarySubtitleId
                text: model.text
                onTriggered: mpv.secondarySubtitleId = model.id
            }
        }
    }

    MenuSeparator {}

    MenuItem { action: actions["subtitleQuickenAction"] }
    MenuItem { action: actions["subtitleDelayAction"] }
    MenuItem { action: actions["subtitleToggleAction"] }
}
