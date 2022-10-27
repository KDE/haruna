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

    title: i18nc("@title:menu", "&Subtitles")

    Labs.Menu {
        id: primarySubtitleMenu

        title: i18nc("@title:menu", "Primary Subtitle")
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

        title: i18nc("@title:menu", "Secondary Subtitle")
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
        icon.name: appActions.openSubtitlesFileAction.icon.name
        text: appActions.openSubtitlesFileAction.text
        onTriggered: appActions.openSubtitlesFileAction.trigger()
    }

    Labs.MenuItem {
        icon.name: appActions.subtitleIncreaseFontSizeAction.icon.name
        text: appActions.subtitleIncreaseFontSizeAction.text
        onTriggered: appActions.subtitleIncreaseFontSizeAction.trigger()
    }

    Labs.MenuItem {
        icon.name: appActions.subtitleDecreaseFontSizeAction.icon.name
        text: appActions.subtitleDecreaseFontSizeAction.text
        onTriggered: appActions.subtitleDecreaseFontSizeAction.trigger()
    }

    Labs.MenuItem {
        icon.name: appActions.subtitleMoveUpAction.icon.name
        text: appActions.subtitleMoveUpAction.text
        onTriggered: appActions.subtitleMoveUpAction.trigger()
    }

    Labs.MenuItem {
        icon.name: appActions.subtitleMoveDownAction.icon.name
        text: appActions.subtitleMoveDownAction.text
        onTriggered: appActions.subtitleMoveDownAction.trigger()
    }

    Labs.MenuItem {
        icon.name: appActions.subtitleQuickenAction.icon.name
        text: appActions.subtitleQuickenAction.text
        onTriggered: appActions.subtitleQuickenAction.trigger()
    }

    Labs.MenuItem {
        icon.name: appActions.subtitleDelayAction.icon.name
        text: appActions.subtitleDelayAction.text
        onTriggered: appActions.subtitleDelayAction.trigger()
    }

    Labs.MenuItem {
        icon.name: appActions.subtitleToggleAction.icon.name
        text: appActions.subtitleToggleAction.text
        onTriggered: appActions.subtitleToggleAction.trigger()
    }

    Labs.MenuItem {
        icon.name: appActions.subtitleCycleUpAction.icon.name
        text: appActions.subtitleCycleUpAction.text
        onTriggered: appActions.subtitleCycleUpAction.trigger()
    }

    Labs.MenuItem {
        icon.name: appActions.subtitleCycleDownAction.icon.name
        text: appActions.subtitleCycleDownAction.text
        onTriggered: appActions.subtitleCycleDownAction.trigger()
    }

}
