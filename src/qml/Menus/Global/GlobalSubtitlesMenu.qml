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

    title: i18nc("@title:menu", "&Subtitles")

    Labs.Menu {
        id: primarySubtitleMenu

        title: i18nc("@title:menu", "Primary Subtitle")
        enabled: mpv.subtitleTracksModel.rowCount() > 1

        Instantiator {
            model: mpv.subtitleTracksModel
            delegate: Labs.MenuItem {
                enabled: model.id !== mpv.secondarySubtitleId || model.id === 0
                checkable: true
                checked: model.id === mpv.subtitleId
                text: model.text
                onTriggered: {
                    mpv.subtitleId = model.id
                }
            }
            onObjectAdded: function(index, object) {
                primarySubtitleMenu.insertItem(index, object)
            }
            onObjectRemoved: function(index, object) {
                primarySubtitleMenu.removeItem(object)
            }
        }
    }

    Labs.Menu {
        id: secondarySubtitleMenu

        title: i18nc("@title:menu", "Secondary Subtitle")
        enabled: mpv.subtitleTracksModel.rowCount() > 1

        Instantiator {
            model: mpv.subtitleTracksModel
            delegate: Labs.MenuItem {
                enabled: model.id !== mpv.subtitleId || model.id === 0
                checkable: true
                checked: model.id === mpv.secondarySubtitleId
                text: model.text
                onTriggered: {
                    mpv.secondarySubtitleId = model.id
                }
            }
            onObjectAdded: function(index, object) {
                secondarySubtitleMenu.insertItem(index, object)
            }
            onObjectRemoved: function(index, object) {
                secondarySubtitleMenu.removeItem(object)
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
