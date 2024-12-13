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

    title: i18nc("@title:menu", "&Subtitles")

    Labs.Menu {
        id: primarySubtitleMenu

        title: i18nc("@title:menu", "Primary Subtitle")
        enabled: root.m_mpv.subtitleTracksModel.rowCount > 1

        Instantiator {
            model: root.m_mpv.subtitleTracksModel
            delegate: Labs.MenuItem {
                id: primarySubDelegate

                required property string displayText
                required property string language
                required property string title
                required property string codec
                required property string trackId

                enabled: primarySubDelegate.trackId !== root.m_mpv.secondarySubtitleId || primarySubDelegate.trackId === 0
                checkable: true
                checked: primarySubDelegate.trackId === root.m_mpv.subtitleId
                text: primarySubDelegate.displayText
                onTriggered: {
                    root.m_mpv.subtitleId = primarySubDelegate.trackId
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
        enabled: root.m_mpv.subtitleTracksModel.rowCount > 1

        Instantiator {
            model: root.m_mpv.subtitleTracksModel
            delegate: Labs.MenuItem {
                id: secondarySubDelegate

                required property string displayText
                required property string language
                required property string title
                required property string codec
                required property string trackId

                enabled: secondarySubDelegate.trackId !== root.m_mpv.subtitleId || secondarySubDelegate.trackId === 0
                checkable: true
                checked: secondarySubDelegate.trackId === root.m_mpv.secondarySubtitleId
                text: secondarySubDelegate.displayText
                onTriggered: {
                    root.m_mpv.secondarySubtitleId = secondarySubDelegate.trackId
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
