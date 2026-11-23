/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 * SPDX-FileCopyrightText: 2025 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

import org.kde.ki18n
import org.kde.kirigami as Kirigami

import org.kde.haruna
import org.kde.haruna.playlist

ListView {
    id: root

    required property PlaylistsManager manager
    required property MpvItem mpvItem
    property bool startupScrollDone: false

    function scrollToCurrentItem() : void {
        const index = root.manager.visiblePlaylist.getPlayingItem()
        if (index === -1) {
            const index = root.manager.visiblePlaylist.getLastPlayedItem()
            root.positionViewAtIndex(index, ListView.Beginning)
        }

        root.positionViewAtIndex(index, ListView.Beginning)
    }

    // set bottomMargin so that the footer doesn't block playlist items
    bottomMargin: 100

    model: root.manager.visiblePlaylist
    onModelChanged: {
        Qt.callLater(root.manager.visiblePlaylist.refreshData)
    }

    reuseItems: true
    spacing: 1
    currentIndex: root.manager.visiblePlaylist.getPlayingItem()
    highlightFollowsCurrentItem: false

    moveDisplaced: Transition {
        NumberAnimation {
            properties: "y"
            duration: Kirigami.Units.shortDuration
        }
    }

    delegate: PlaylistItem {
        mpvItem: root.mpvItem
        playlistsManager: root.manager
    }

    section {
        property: "section"
        delegate: PlaylistSectionDelegate {
            model: root.manager.visiblePlaylist
        }
    }

    onCountChanged: {
        if (startupScrollDone || count <= 0) {
            return
        }
        startupScrollDone = true
        Qt.callLater(positionViewAtIndex, currentIndex, ListView.Beginning)
    }

    ContextMenu.menu: Menu {
        MenuItem {
            text: KI18n.i18nc("@action:inmenu", "Select All")
            onClicked: root.manager.visiblePlaylist.selectItem(0, PlaylistFilterProxyModel.All)
        }

        MenuItem {
            text: KI18n.i18nc("@action:inmenu", "Deselect All")
            onClicked: root.manager.visiblePlaylist.selectItem(0, PlaylistFilterProxyModel.Clear)
        }

        MenuItem {
            text: KI18n.i18nc("@action:inmenu", "Invert Selection")
            onClicked: root.manager.visiblePlaylist.selectItem(0, PlaylistFilterProxyModel.Invert)
        }

        MenuSeparator {}

        MenuItem {
            text: KI18n.i18nc("@action:inmenu", "Scroll to Playing Item")
            onClicked: root.scrollToCurrentItem()
        }
    }

    TapHandler {
        acceptedButtons: Qt.MiddleButton
        onSingleTapped: root.scrollToCurrentItem()
    }
}
