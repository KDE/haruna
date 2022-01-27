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

    title: i18n("&Subtitles")

    Labs.Menu {
        id: primarySubtitleMenu

        title: i18n("Primary Subtitle")
        onAboutToShow: primarySubtitleMenuInstantiator.model = mpv.subtitleTracksModel

        Instantiator {
            id: primarySubtitleMenuInstantiator
            model: 0
            onObjectAdded: primarySubtitleMenu.insertItem( index, object )
            onObjectRemoved: primarySubtitleMenu.removeItem( object )
            delegate: Labs.MenuItem {
                enabled: model.id !== mpv.secondarySubtitleId || model.id === 0
                checkable: true
                checked: model.id === mpv.subtitleId
                text: model.text
                onTriggered: mpv.subtitleId = model.id
            }
        }
    }

    Labs.Menu {
        id: secondarySubtitleMenu

        title: i18n("Secondary Subtitle")
        onAboutToShow: secondarySubtitleMenuInstantiator.model = mpv.subtitleTracksModel

        Instantiator {
            id: secondarySubtitleMenuInstantiator
            model: 0
            onObjectAdded: secondarySubtitleMenu.insertItem( index, object )
            onObjectRemoved: secondarySubtitleMenu.removeItem( object )
            delegate: Labs.MenuItem {
                enabled: model.id !== mpv.subtitleId || model.id === 0
                checkable: true
                checked: model.id === mpv.secondarySubtitleId
                text: model.text
                onTriggered: mpv.secondarySubtitleId = model.id
            }
        }
    }

    Labs.MenuSeparator {}

    Labs.MenuItem {
        icon.name: actions.increaseSubtitleFontSizeAction.icon.name
        text: actions.increaseSubtitleFontSizeAction.text
        onTriggered: actions.increaseSubtitleFontSizeAction.trigger()
    }

    Labs.MenuItem {
        icon.name: actions.decreaseSubtitleFontSizeAction.icon.name
        text: actions.decreaseSubtitleFontSizeAction.text
        onTriggered: actions.decreaseSubtitleFontSizeAction.trigger()
    }

    Labs.MenuItem {
        icon.name: actions.subtitlePositionUpAction.icon.name
        text: actions.subtitlePositionUpAction.text
        onTriggered: actions.subtitlePositionUpAction.trigger()
    }

    Labs.MenuItem {
        icon.name: actions.subtitlePositionDownAction.icon.name
        text: actions.subtitlePositionDownAction.text
        onTriggered: actions.subtitlePositionDownAction.trigger()
    }

    Labs.MenuItem {
        icon.name: actions.subtitleQuickenAction.icon.name
        text: actions.subtitleQuickenAction.text
        onTriggered: actions.subtitleQuickenAction.trigger()
    }

    Labs.MenuItem {
        icon.name: actions.subtitleDelayAction.icon.name
        text: actions.subtitleDelayAction.text
        onTriggered: actions.subtitleDelayAction.trigger()
    }

    Labs.MenuItem {
        icon.name: actions.subtitleToggleAction.icon.name
        text: actions.subtitleToggleAction.text
        onTriggered: actions.subtitleToggleAction.trigger()
    }

    Labs.MenuItem {
        icon.name: actions.subtitleCycleUpAction.icon.name
        text: actions.subtitleCycleUpAction.text
        onTriggered: actions.subtitleCycleUpAction.trigger()
    }

    Labs.MenuItem {
        icon.name: actions.subtitleCycleDownAction.icon.name
        text: actions.subtitleCycleDownAction.text
        onTriggered: actions.subtitleCycleDownAction.trigger()
    }

}
