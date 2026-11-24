/*
 * SPDX-FileCopyrightText: 2025 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import org.kde.ki18n

import org.kde.kirigami as Kirigami
import org.kde.haruna
import org.kde.haruna.playlist

Loader {
    id: root

    required property PlaylistsManager playlistsManager

    function open() : void {
        if (!root.active) {
            root.active = true
            root.loaded.connect(function() {
                root.open()
            })
            return
        }

        const window = root.item as Kirigami.ApplicationWindow
        if (window.visible) {
            window.raise()
        } else {
            window.visible = true
        }
    }

    active: false
    asynchronous: true
    sourceComponent: Kirigami.ApplicationWindow {
        id: window

        Component.onCompleted: calculatePostLayout()

        width: 550
        height: 600
        minimumWidth: 250
        minimumHeight: 250
        title: KI18n.i18nc("@title:configure sort window", "Advanced Sort and Group By")
        transientParent: null

        Shortcut {
            sequence: "Esc"
            onActivated: root.active = false
        }

        GridLayout {
            columns: 2
            anchors.fill: parent
            anchors.margins: Kirigami.Units.largeSpacing

            Kirigami.Heading {
                text: KI18n.i18nc("@label which playlist is affected, %1 is the playlist name",
                                  "Playlist: %1", root.playlistsManager.visiblePlaylist.playlistName())
                Layout.columnSpan: 2
            }

            Kirigami.ListSectionHeader {
                text: KI18n.i18nc("@label sort", "Sorting")
                Layout.columnSpan: 2
                Layout.fillWidth: true
            }

            FilterField {
                id: availableSortSearch
                model: root.playlistsManager.visiblePlaylist.availableSortPropertiesProxyModel()
            }

            Kirigami.ActionToolBar {
                alignment: Qt.AlignLeft
                Layout.margins: Kirigami.Units.largeSpacing
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft

                actions: [
                    Kirigami.Action {
                        text: {
                            switch(root.playlistsManager.visiblePlaylist.sortOrder){
                                case Qt.AscendingOrder:
                                return actionAscending.text
                                case Qt.DescendingOrder:
                                return actionDescending.text
                                default:
                                return ""
                            }
                        }

                        icon.name: {
                            switch(root.playlistsManager.visiblePlaylist.sortOrder){
                                case Qt.AscendingOrder:
                                return "view-sort-ascending-name"
                                case Qt.DescendingOrder:
                                return "view-sort-descending-name"
                                default:
                                return ""
                            }
                        }

                        displayHint: Kirigami.DisplayHint.KeepVisible

                        Kirigami.Action {
                            id: actionAscending

                            text: KI18n.i18nc("@action:inmenu", "Ascending")
                            autoExclusive: true
                            checkable: true
                            checked: root.playlistsManager.visiblePlaylist.sortOrder === Qt.AscendingOrder
                            icon.name: "view-sort-ascending-name"

                            onTriggered: {
                                root.playlistsManager.visiblePlaylist.sortOrder = Qt.AscendingOrder
                            }
                        }
                        Kirigami.Action {
                            id: actionDescending

                            text: KI18n.i18nc("@action:inmenu", "Descending")
                            autoExclusive: true
                            checkable: true
                            checked: root.playlistsManager.visiblePlaylist.sortOrder === Qt.DescendingOrder
                            icon.name: "view-sort-descending-name"

                            onTriggered: {
                                root.playlistsManager.visiblePlaylist.sortOrder = Qt.DescendingOrder
                            }
                        }
                    }
                ]
            }


            ScrollView {
                id: availableSortScrollView

                z: 10
                clip: true

                background: Rectangle {
                    radius: Kirigami.Units.cornerRadius
                    color: Kirigami.Theme.backgroundColor

                    border {
                        width: 1
                        color: Qt.alpha(Kirigami.Theme.textColor, 0.25)
                    }
                }

                Kirigami.Theme.colorSet: Kirigami.Theme.View
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: parent.width / 2
                Layout.preferredHeight: parent.height
                ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
                ScrollBar.vertical.policy: ScrollBar.AlwaysOff

                SortListView {
                    scrollbarWidth: availableSortScrollView.effectiveScrollBarWidth
                    isActiveModel: false
                    isGroup: false

                    model: root.playlistsManager.visiblePlaylist.availableSortPropertiesProxyModel()
                    delegate: PropertyDelegate {}

                    displaced: Transition {
                        NumberAnimation {
                            properties: "y"
                            duration: Kirigami.Units.shortDuration
                        }
                    }
                }
            }

            ScrollView {
                id: activeSortScrollView

                z: 10
                clip: true

                background: Rectangle {
                    radius: Kirigami.Units.cornerRadius
                    color: Kirigami.Theme.backgroundColor
                    border {
                        width: 1
                        color: Qt.alpha(Kirigami.Theme.textColor, 0.25)
                    }
                }

                Kirigami.Theme.colorSet: Kirigami.Theme.View
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: parent.width / 2
                Layout.preferredHeight: parent.height
                ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
                ScrollBar.vertical.policy: ScrollBar.AlwaysOff

                SortListView {
                    id: activeListView

                    scrollbarWidth: activeSortScrollView.effectiveScrollBarWidth
                    isActiveModel: true
                    isGroup: false

                    model: root.playlistsManager.visiblePlaylist.activeSortPropertiesModel()
                    delegate: PropertyDelegate {}

                    displaced: Transition {
                        NumberAnimation {
                            properties: "y"
                            duration: Kirigami.Units.shortDuration
                        }
                    }
                }
            }

            Item {
                Layout.columnSpan: 2
                Layout.topMargin: Kirigami.Units.largeSpacing
            }

            Kirigami.ListSectionHeader {
                text: KI18n.i18nc("@label group", "Grouping")
                Layout.columnSpan: 2
                Layout.fillWidth: true
            }

            FilterField {
                id: availableGroupSearch
                model: root.playlistsManager.visiblePlaylist.availableGroupProxyModel()
            }

            RowLayout {
                Layout.margins: Kirigami.Units.largeSpacing
                CheckBox {
                    text: KI18n.i18nc("@action:inmenu", "Show Sections")
                    checked: root.playlistsManager.visiblePlaylist.showSections

                    onCheckedChanged: {
                        root.playlistsManager.visiblePlaylist.showSections = checked
                    }

                }
                ToolTipButton {
                    toolTipText: KI18n.i18nc("@info:tooltip",
                                             "Show/Hide section labels in the playlist view. <br>" +
                                             "If the playlist is not sorted by the included group properties in the " +
                                             "same order, then the sections might not be unique.")
                    toolTipWidth: 450
                    Layout.preferredHeight: Kirigami.Units.iconSizes.medium
                }
            }

            ScrollView {
                id: availableGroupScrollView

                z: 10
                clip: true

                background: Rectangle {
                    radius: Kirigami.Units.cornerRadius
                    color: Kirigami.Theme.backgroundColor

                    border {
                        width: 1
                        color: Qt.alpha(Kirigami.Theme.textColor, 0.25)
                    }
                }

                Kirigami.Theme.colorSet: Kirigami.Theme.View
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: parent.width / 2
                Layout.preferredHeight: parent.height
                ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
                ScrollBar.vertical.policy: ScrollBar.AlwaysOff

                SortListView {
                    scrollbarWidth: availableGroupScrollView.effectiveScrollBarWidth
                    isActiveModel: false
                    isGroup: true

                    model: root.playlistsManager.visiblePlaylist.availableGroupProxyModel()
                    delegate: PropertyDelegate {}

                    displaced: Transition {
                        NumberAnimation {
                            properties: "y"
                            duration: Kirigami.Units.shortDuration
                        }
                    }
                }
            }

            ColumnLayout {
                spacing: 0
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.preferredHeight: parent.height
                Layout.preferredWidth: parent.width / 2

                ScrollView {
                    id: activeGroupScrollView

                    z: 10
                    clip: true

                    background: Rectangle {
                        topLeftRadius: Kirigami.Units.cornerRadius
                        topRightRadius: Kirigami.Units.cornerRadius
                        color: Kirigami.Theme.backgroundColor
                        border {
                            width: 1
                            color: Qt.alpha(Kirigami.Theme.textColor, 0.25)
                        }
                    }

                    Kirigami.Theme.colorSet: Kirigami.Theme.View
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
                    ScrollBar.vertical.policy: ScrollBar.AlwaysOff

                    SortListView {
                        id: activeGroupListView

                        scrollbarWidth: activeGroupScrollView.effectiveScrollBarWidth
                        isActiveModel: true
                        isGroup: true

                        model: root.playlistsManager.visiblePlaylist.activeGroupModel()
                        delegate: PropertyDelegate {}

                        displaced: Transition {
                            NumberAnimation {
                                properties: "y"
                                duration: Kirigami.Units.shortDuration
                            }
                        }
                    }
                }

                ToolButton {
                    icon.name: "newline"
                    text: KI18n.i18nc("@action:inmenu add new line button", "Add New Line")
                    Layout.alignment: Qt.AlignRight

                    onClicked: {
                        root.playlistsManager.visiblePlaylist.addToActiveGroup(PlaylistSortProxyModel.Separator)
                    }
                }

            }
        }

        function calculatePostLayout() {
            // effectiveScrollBarWidth is always 0 at load time because of the Loader.
            // We set vertical scroll policy to ScrollBar.AlwaysOff initially then
            // set them back to ScrollBar.AsNeeded after the item is loaded via this function
            availableSortScrollView.ScrollBar.vertical.policy = ScrollBar.AsNeeded
            activeSortScrollView.ScrollBar.vertical.policy = ScrollBar.AsNeeded
            availableGroupScrollView.ScrollBar.vertical.policy = ScrollBar.AsNeeded
            activeGroupScrollView.ScrollBar.vertical.policy = ScrollBar.AsNeeded
        }

    }
    component SortListView: ListView {
        required property real scrollbarWidth
        required property bool isActiveModel
        required property bool isGroup
    }

    component FilterField: Kirigami.SearchField {
        id: filterField

        required property PlaylistSortPropertyProxyModel model

        placeholderText: "Search"
        delaySearch: true
        Layout.preferredWidth: parent.width / 2
        Layout.fillWidth: true

        onTextChanged: {
            filterField.model.searchText = text
        }

        Component.onCompleted: {
            const model = root.playlistsManager.visiblePlaylist.availableSortPropertiesProxyModel()
            text = filterField.model.searchText
        }

        rightActions: [
            Kirigami.Action {
                text: KI18n.i18nc("@action:inmenu", "Filter")

                icon {
                    name: "view-filter-symbolic"
                    width: Kirigami.Units.iconSizes.medium
                    height: Kirigami.Units.iconSizes.medium
                }

                onTriggered: {
                    sortFilterMenu.open()
                }
            }
        ]

        Menu {
            id: sortFilterMenu

            x: filterField.width - Kirigami.Units.iconSizes.medium
            y: filterField.height
            title: KI18n.i18nc("@title:menu", "Filter")

            ActionGroup { id: sortFilterActionGroup }

            Kirigami.Action {
                checked: true
                checkable: true
                text: KI18n.i18nc("@action:inmenu all", "All")

                onTriggered: {
                    filterField.model.filterCategory = PlaylistSortPropertyModel.All
                }

                ActionGroup.group: sortFilterActionGroup
            }
            Kirigami.Action {
                checkable: true
                text: KI18n.i18nc("@action:inmenu file", "File")

                onTriggered: {
                    filterField.model.filterCategory = PlaylistSortPropertyModel.FileCategory
                }

                ActionGroup.group: sortFilterActionGroup
            }
            Kirigami.Action {
                checkable: true
                text: KI18n.i18nc("@action:inmenu audio", "Audio")

                onTriggered: {
                    filterField.model.filterCategory = PlaylistSortPropertyModel.AudioCategory
                }

                ActionGroup.group: sortFilterActionGroup
            }
            Kirigami.Action {
                checkable: true
                text: KI18n.i18nc("@action:inmenu video", "Video")

                onTriggered: {
                    filterField.model.filterCategory = PlaylistSortPropertyModel.VideoCategory
                }

                ActionGroup.group: sortFilterActionGroup
            }
        }
    }

    component PropertyDelegate: Loader {
        id: propertyDelegate

        required property int index
        required property string label
        required property int sort
        required property int category
        required property int order
        required property bool hideBlank
        // which listview is this delegate is in?
        readonly property SortListView view: ListView.view as SortListView
        readonly property bool isActiveModel: view.isActiveModel
        readonly property bool isGroup: view.isGroup

        function remove() : void {
            if (!propertyDelegate.isActiveModel) {
                return
            }

            const playlist = root.playlistsManager.visiblePlaylist
            if (propertyDelegate.isGroup) {
                playlist.removeFromActiveGroup(propertyDelegate.index)
            } else {
                playlist.removeFromActiveSortProperties(propertyDelegate.index)
            }
        }

        function add() : void {
            if (propertyDelegate.isActiveModel) {
                return
            }

            const playlist = root.playlistsManager.visiblePlaylist
            if (propertyDelegate.isGroup) {
                playlist.addToActiveGroup(propertyDelegate.sort)
            } else {
                playlist.addToActiveSortProperties(propertyDelegate.sort)
            }
        }

        function iconName() : string {
            switch(propertyDelegate.category) {
            case (PlaylistSortPropertyModel.FileCategory):
                return "folder-documents-symbolic"
            case (PlaylistSortPropertyModel.AudioCategory):
                switch(propertyDelegate.sort) {
                case (PlaylistSortProxyModel.Genre):
                    return "view-media-genre"
                case (PlaylistSortProxyModel.Album):
                    return "media-album-cover"
                case (PlaylistSortProxyModel.Artist):
                case (PlaylistSortProxyModel.AlbumArtist):
                case (PlaylistSortProxyModel.Composer):
                case (PlaylistSortProxyModel.Lyricist):
                    return "view-media-artist-symbolic"
                case (PlaylistSortProxyModel.DiscNo):
                    return "media-optical-dvd-symbolic"
                default:
                    return "folder-music-symbolic"
                }
            case (PlaylistSortPropertyModel.VideoCategory):
                return "folder-video-symbolic"
            case (PlaylistSortPropertyModel.SeparatorCategory):
                return ""
            default:
                return ""
            }
        }

        sourceComponent: ItemDelegate {
            implicitWidth: propertyDelegate.view.width
            text: propertyDelegate.label
            onClicked: propertyDelegate.add()
            contentItem: RowLayout {
                spacing: 0

                Kirigami.ListItemDragHandle {
                    id: itemDelegateDragHandle

                    visible: propertyDelegate.isActiveModel
                    enabled: propertyDelegate.isActiveModel
                    listItem: propertyDelegate.item as Item
                    listView: propertyDelegate.view

                    onMoveRequested: function(oldIndex, newIndex) {
                        let model
                        if (propertyDelegate.isGroup) {
                            model = root.playlistsManager.visiblePlaylist.activeGroupModel()
                        }
                        else {
                            model = root.playlistsManager.visiblePlaylist.activeSortPropertiesModel()
                        }
                        const sortModel = model as PlaylistSortPropertyModel
                        sortModel.moveSortProperty(oldIndex, newIndex)
                    }

                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    Layout.rightMargin: Kirigami.Units.smallSpacing
                }

                Kirigami.IconTitleSubtitle {
                    title: propertyDelegate.label
                    icon.name: propertyDelegate.iconName()

                    Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.leftMargin: propertyDelegate.isActiveModel ? 0 : Kirigami.Units.largeSpacing
                }

                ToolButton {
                    visible: propertyDelegate.isActiveModel
                    icon.width: Kirigami.Units.iconSizes.small
                    icon.height: Kirigami.Units.iconSizes.small
                    icon.name: {
                        if (propertyDelegate.isGroup) {
                            if (propertyDelegate.hideBlank){
                                return "view-visible-off"
                            } else {
                                return "view-visible"
                            }
                        } else {
                            switch(propertyDelegate.order){
                            case (PlaylistSortPropertyModel.Ascending):
                                return "view-sort-ascending-name"
                            case (PlaylistSortPropertyModel.Descending):
                                return "view-sort-descending-name"
                            case (PlaylistSortPropertyModel.SameAsPrimary):
                                return "view-sort"
                            default:
                                return ""
                            }
                        }
                    }
                    onClicked: {
                        const playlist = root.playlistsManager.visiblePlaylist
                        const row = propertyDelegate.index
                        if (propertyDelegate.isGroup) {
                            playlist.setGroupHideBlank(row, !propertyDelegate.hideBlank)
                            return
                        } else {
                            let sortOrder = PlaylistSortPropertyModel.Ascending
                            switch(propertyDelegate.order){
                            case (PlaylistSortPropertyModel.Ascending):
                                sortOrder = PlaylistSortPropertyModel.Descending
                                break
                            case (PlaylistSortPropertyModel.Descending):
                                sortOrder = PlaylistSortPropertyModel.SameAsPrimary
                                break
                            case (PlaylistSortPropertyModel.SameAsPrimary):
                                sortOrder = PlaylistSortPropertyModel.Ascending
                                break
                            default:
                                sortOrder = PlaylistSortPropertyModel.Ascending
                                break
                            }
                            playlist.setSortPropertySortingOrder(row, sortOrder)
                        }
                    }

                    ToolTip.visible: hovered
                    ToolTip.delay: Kirigami.Units.shortDuration
                    ToolTip.text: {
                        if (propertyDelegate.isGroup) {
                            if (propertyDelegate.hideBlank) {
                                return KI18n.i18nc("@info:tooltip show blank", "Property is hidden if empty or not unique.")
                            } else {
                                return KI18n.i18nc("@info:tooltip hide blank", "Property is shown, if empty a placeholder will be shown.")
                            }
                        } else {
                            switch(propertyDelegate.order){
                            case (PlaylistSortPropertyModel.Ascending):
                                return KI18n.i18nc("@info:tooltip group order", "Sort ascending")
                            case (PlaylistSortPropertyModel.Descending):
                                return KI18n.i18nc("@info:tooltip group order", "Sort descending")
                            case (PlaylistSortPropertyModel.SameAsPrimary):
                                return KI18n.i18nc("@info:tooltip group order", "Same as primary sort order")
                            default:
                                return ""
                            }
                        }
                    }
                }

                ToolButton {
                    visible: propertyDelegate.isActiveModel
                    icon.width: Kirigami.Units.iconSizes.small
                    icon.height: Kirigami.Units.iconSizes.small
                    icon.name: "delete"
                    onClicked: propertyDelegate.remove()
                }
            }
            TapHandler {
                acceptedButtons: Qt.MiddleButton
                onTapped: propertyDelegate.remove()
            }
        }
    }
}
