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

    title: i18nc("@title:menu", "&Subtitles")

    Menu {
        id: primarySubtitleMenu

        title: i18nc("@title:menu", "Primary Subtitle")

        Instantiator {
            id: primarySubtitleMenuInstantiator
            model: mpv.subtitleTracksModel
            onObjectAdded: primarySubtitleMenu.insertItem( index, object )
            onObjectRemoved: primarySubtitleMenu.removeItem( object )
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

        title: i18nc("@title:menu", "Secondary Subtitle")

        Instantiator {
            id: secondarySubtitleMenuInstantiator
            model: mpv.subtitleTracksModel
            onObjectAdded: secondarySubtitleMenu.insertItem( index, object )
            onObjectRemoved: secondarySubtitleMenu.removeItem( object )
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

    MenuItem { action: appActions.openSubtitlesFileAction }
    MenuItem { action: appActions.subtitleIncreaseFontSizeAction }
    MenuItem { action: appActions.subtitleDecreaseFontSizeAction }
    MenuItem { action: appActions.subtitleMoveUpAction }
    MenuItem { action: appActions.subtitleMoveDownAction }
    MenuItem { action: appActions.subtitleQuickenAction }
    MenuItem { action: appActions.subtitleDelayAction }
    MenuItem { action: appActions.subtitleToggleAction }
    MenuItem { action: appActions.subtitleCycleUpAction }
    MenuItem { action: appActions.subtitleCycleDownAction }
}
