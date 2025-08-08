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
        icon.name: HarunaApp.actions.addExternalSubtitleAction.icon.name
        text: HarunaApp.actions.addExternalSubtitleAction.text
        onTriggered: HarunaApp.actions.addExternalSubtitleAction.trigger()
    }

    Labs.MenuItem {
        icon.name: HarunaApp.actions.subtitleIncreaseFontSizeAction.icon.name
        text: HarunaApp.actions.subtitleIncreaseFontSizeAction.text
        onTriggered: HarunaApp.actions.subtitleIncreaseFontSizeAction.trigger()
    }

    Labs.MenuItem {
        icon.name: HarunaApp.actions.subtitleDecreaseFontSizeAction.icon.name
        text: HarunaApp.actions.subtitleDecreaseFontSizeAction.text
        onTriggered: HarunaApp.actions.subtitleDecreaseFontSizeAction.trigger()
    }

    Labs.MenuItem {
        icon.name: HarunaApp.actions.subtitleMoveUpAction.icon.name
        text: HarunaApp.actions.subtitleMoveUpAction.text
        onTriggered: HarunaApp.actions.subtitleMoveUpAction.trigger()
    }

    Labs.MenuItem {
        icon.name: HarunaApp.actions.subtitleMoveDownAction.icon.name
        text: HarunaApp.actions.subtitleMoveDownAction.text
        onTriggered: HarunaApp.actions.subtitleMoveDownAction.trigger()
    }

    Labs.MenuItem {
        icon.name: HarunaApp.actions.subtitleQuickenAction.icon.name
        text: HarunaApp.actions.subtitleQuickenAction.text
        onTriggered: HarunaApp.actions.subtitleQuickenAction.trigger()
    }

    Labs.MenuItem {
        icon.name: HarunaApp.actions.subtitleDelayAction.icon.name
        text: HarunaApp.actions.subtitleDelayAction.text
        onTriggered: HarunaApp.actions.subtitleDelayAction.trigger()
    }

    Labs.MenuItem {
        icon.name: HarunaApp.actions.subtitleToggleAction.icon.name
        text: HarunaApp.actions.subtitleToggleAction.text
        onTriggered: HarunaApp.actions.subtitleToggleAction.trigger()
    }

    Labs.MenuItem {
        icon.name: HarunaApp.actions.subtitleCycleUpAction.icon.name
        text: HarunaApp.actions.subtitleCycleUpAction.text
        onTriggered: HarunaApp.actions.subtitleCycleUpAction.trigger()
    }

    Labs.MenuItem {
        icon.name: HarunaApp.actions.subtitleCycleDownAction.icon.name
        text: HarunaApp.actions.subtitleCycleDownAction.text
        onTriggered: HarunaApp.actions.subtitleCycleDownAction.trigger()
    }

}
