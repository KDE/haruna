/*
 * SPDX-FileCopyrightText: 2025 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 * SPDX-FileCopyrightText: 2026 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

import org.kde.ki18n

import org.kde.haruna

TabBar {
    id: root

    required property MpvVideo m_mpv

    function openContextMenu(tab) {
        tab = tab as PlaylistTabDelegate
        if (!tab) {
            return
        }
        tabMenuLoader.open(tab)
    }

    // Magic number 3: tabMenuLoader has 3 default items
    // Playlist is placed below them, so we have to offset this by 3
    function movePlaylistItem(from, to) {
        moveItem(from, to)
        var contextMenu = tabMenuLoader.item as Menu

        if (!contextMenu) {
            return
        }

        if (contextMenu.count > 3) {
            contextMenu.moveItem(from + 3, to + 3)
        }
    }

    clip: true

    onCurrentIndexChanged: {
        root.m_mpv.playlists.visibleIndex = currentIndex
    }

    // This connection is only necessary for the startup. If the last played item is inside
    // an internal tab, then the TabBar should set its current index to that tab.
    Connections {
        target: root.m_mpv.playlists
        function onVisibleIndexChanged() {
            root.currentIndex = root.m_mpv.playlists.visibleIndex
        }
    }


    Loader {
        id: tabMenuLoader

        property int row: -1

        active: true
        asynchronous: false
        sourceComponent: Menu {
            id: tabMenu

            MenuItem {
                text: KI18n.i18nc("@action:inmenu", "Rename")
                icon.name: "edit-rename"
                visible: tabMenuLoader.row > 0
                onClicked: root.m_mpv.playlists.renamePlaylist(tabMenuLoader.row)
            }
            MenuItem {
                text: KI18n.i18nc("@action:inmenu", "Remove")
                icon.name: "remove"
                visible: tabMenuLoader.row > 0
                onClicked: root.m_mpv.playlists.removePlaylist(tabMenuLoader.row)
            }

            MenuSeparator {
                visible: tabMenuLoader.row > 0
            }

            // Playlists
            Instantiator {
                model: root.m_mpv.playlists
                delegate: MenuItem {
                    required property int index
                    required property string name
                    required property bool isVisible
                    required property bool isActive

                    text: name
                    icon.name: isVisible ? "draw-circle" : ""
                    onClicked: root.setCurrentIndex(index)
                }
                onObjectAdded: (index, object) => tabMenu.insertItem(index + 3, object)
                onObjectRemoved: (index, object) => tabMenu.removeItem(object)
            }
        }

        function open(tab) : void {
            if (!tabMenuLoader.active) {
                tabMenuLoader.active = true
                tabMenuLoader.loaded.connect(function() {
                    tabMenuLoader.open(tab)
                })
                return
            }

            if (!tab) {
                return
            }

            tabMenuLoader.row = tab.index

            const contextMenu = tabMenuLoader.item as Menu
            contextMenu.popup(tab)
        }
    }
}
