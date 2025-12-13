/*
 * SPDX-FileCopyrightText: 2025 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import org.kde.kirigami as Kirigami
import org.kde.haruna
import org.kde.haruna.playlist

Kirigami.ApplicationWindow {
    id: root

    required property MpvItem m_mpv

    width: 550
    height: 600
    minimumWidth: 250
    minimumHeight: 250
    title: i18nc("@title:configure sort window", "Advanced Sort and Group By")
    transientParent: null

    Shortcut {
        sequence: "Esc"
        onActivated: root.close()
    }

    GridLayout {
        columns: 2
        anchors.fill: parent
        anchors.margins: Kirigami.Units.largeSpacing

        SettingsHeader {
            Layout.columnSpan: 2
            headingLevel: 1
            text: i18nc("@label which playlist is affected, %1 is the playlist name",
                        "Playlist: %1", root.m_mpv.visibleFilterProxyModel.playlistName())
            topMargin: Kirigami.Units.smallSpacing
        }

        SettingsHeader {
            Layout.columnSpan: 2
            headingLevel: 2
            text: i18nc("@label sort", "Sorting")
            topMargin: Kirigami.Units.smallSpacing
        }


        Kirigami.SearchField {
            id: availableSortSearch

            placeholderText: "Search"
            delaySearch: true
            Layout.preferredWidth: parent.width / 2
            Layout.fillWidth: true

            onTextChanged: {
                var model = root.m_mpv.visibleFilterProxyModel.availableSortPropertiesProxyModel()
                model.searchText = text
            }

            Component.onCompleted: {
                var model = root.m_mpv.visibleFilterProxyModel.availableSortPropertiesProxyModel()
                text = model.searchText
            }

            rightActions: [
                Kirigami.Action {
                    text: i18nc("@action:inmenu", "Filter")

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

                x: availableSortSearch.width - Kirigami.Units.iconSizes.medium
                y: availableSortSearch.height
                title: i18nc("@action:inmenu", "Filter")

                ActionGroup { id: sortFilterActionGroup }

                Kirigami.Action {
                    checked: true
                    checkable: true
                    text: i18nc("@action:inmenu all", "All")

                    onTriggered: {
                        var model = root.m_mpv.visibleFilterProxyModel.availableSortPropertiesProxyModel()
                        model.filterCategory = PlaylistSortPropertyModel.All
                    }

                    ActionGroup.group: sortFilterActionGroup
                }
                Kirigami.Action {
                    checkable: true
                    text: i18nc("@action:inmenu file", "File")

                    onTriggered: {
                        var model = root.m_mpv.visibleFilterProxyModel.availableSortPropertiesProxyModel()
                        model.filterCategory = PlaylistSortPropertyModel.FileCategory
                    }

                    ActionGroup.group: sortFilterActionGroup
                }
                Kirigami.Action {
                    checkable: true
                    text: i18nc("@action:inmenu audio", "Audio")

                    onTriggered: {
                        var model = root.m_mpv.visibleFilterProxyModel.availableSortPropertiesProxyModel()
                        model.filterCategory = PlaylistSortPropertyModel.AudioCategory
                    }

                    ActionGroup.group: sortFilterActionGroup
                }
                Kirigami.Action {
                    checkable: true
                    text: i18nc("@action:inmenu video", "Video")

                    onTriggered: {
                        const model = root.m_mpv.visibleFilterProxyModel.availableSortPropertiesProxyModel()
                        model.filterCategory = PlaylistSortPropertyModel.VideoCategory
                    }

                    ActionGroup.group: sortFilterActionGroup
                }
            }
        }

        Kirigami.ActionToolBar {
            alignment: Qt.AlignLeft
            Layout.margins: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignLeft

            actions: [
                Kirigami.Action {
                    text: {
                        switch(root.m_mpv.visibleFilterProxyModel.sortOrder){
                            case Qt.AscendingOrder:
                            return actionAscending.text
                            case Qt.DescendingOrder:
                            return actionDescending.text
                            default:
                            return ""
                        }
                    }

                    icon.name: {
                        switch(root.m_mpv.visibleFilterProxyModel.sortOrder){
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

                        text: i18nc("@action:inmenu", "Ascending")
                        autoExclusive: true
                        checkable: true
                        checked: root.m_mpv.visibleFilterProxyModel.sortOrder === Qt.AscendingOrder
                        icon.name: "view-sort-ascending-name"

                        onTriggered: {
                            root.m_mpv.visibleFilterProxyModel.sortOrder = Qt.AscendingOrder
                        }
                    }
                    Kirigami.Action {
                        id: actionDescending

                        text: i18nc("@action:inmenu", "Descending")
                        autoExclusive: true
                        checkable: true
                        checked: root.m_mpv.visibleFilterProxyModel.sortOrder === Qt.DescendingOrder
                        icon.name: "view-sort-descending-name"

                        onTriggered: {
                            root.m_mpv.visibleFilterProxyModel.sortOrder = Qt.DescendingOrder
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

            ListView {
                readonly property real scrollbarWidth: availableSortScrollView.effectiveScrollBarWidth
                readonly property bool isActiveModel: false
                readonly property bool isGroup: false

                anchors.fill: parent
                model: root.m_mpv.visibleFilterProxyModel.availableSortPropertiesProxyModel()
                delegate: benchedItemDelegate

                displaced: Transition {
                    NumberAnimation {
                        properties: "y"
                        duration: Kirigami.Units.shortDuration
                    }
                }

                leftMargin: Kirigami.Units.largeSpacing
                rightMargin: Kirigami.Units.largeSpacing
                topMargin: Kirigami.Units.largeSpacing
                bottomMargin: Kirigami.Units.largeSpacing
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

            ListView {
                id: activeListView

                readonly property real scrollbarWidth: activeSortScrollView.effectiveScrollBarWidth
                readonly property bool isActiveModel: true
                readonly property bool isGroup: false

                anchors.fill: parent
                model: root.m_mpv.visibleFilterProxyModel.activeSortPropertiesModel()
                delegate: benchedItemDelegate

                displaced: Transition {
                    NumberAnimation {
                        properties: "y"
                        duration: Kirigami.Units.shortDuration
                    }
                }

                leftMargin: Kirigami.Units.largeSpacing
                rightMargin: Kirigami.Units.largeSpacing
                topMargin: Kirigami.Units.largeSpacing
                bottomMargin: Kirigami.Units.largeSpacing
            }
        }

        Item {
            Layout.columnSpan: 2
            Layout.topMargin: Kirigami.Units.largeSpacing
        }

        SettingsHeader {
            Layout.columnSpan: 2
            headingLevel: 2
            text: i18nc("@label group", "Grouping")
            topMargin: Kirigami.Units.smallSpacing
        }

        Kirigami.SearchField {
            id: availableGroupSearch

            placeholderText: "Search"
            delaySearch: true
            Layout.preferredWidth: parent.width / 2
            Layout.fillWidth: true

            onTextChanged: {
                var model = root.m_mpv.visibleFilterProxyModel.availableGroupProxyModel()
                model.searchText = text
            }

            Component.onCompleted: {
                var model = root.m_mpv.visibleFilterProxyModel.availableGroupProxyModel()
                text = model.searchText
            }

            rightActions: [
                Kirigami.Action {
                    text: i18nc("@action:inmenu", "Filter")

                    icon {
                        name: "view-filter-symbolic"
                        width: Kirigami.Units.iconSizes.medium
                        height: Kirigami.Units.iconSizes.medium
                    }

                    onTriggered: {
                        groupFilterMenu.open()
                    }
                }
            ]

            Menu {
                id: groupFilterMenu

                x: availableGroupSearch.width - Kirigami.Units.iconSizes.medium
                y: availableGroupSearch.height
                title: i18nc("@action:inmenu", "Filter")

                ActionGroup { id: groupFilterActionGroup }

                Kirigami.Action {
                    checked: true
                    checkable: true
                    text: i18nc("@action:inmenu all", "All")

                    onTriggered: {
                        var model = root.m_mpv.visibleFilterProxyModel.availableGroupProxyModel()
                        model.filterCategory = PlaylistSortPropertyModel.All
                    }

                    ActionGroup.group: groupFilterActionGroup
                }
                Kirigami.Action {
                    checkable: true
                    text: i18nc("@action:inmenu file", "File")

                    onTriggered: {
                        var model = root.m_mpv.visibleFilterProxyModel.availableGroupProxyModel()
                        model.filterCategory = PlaylistSortPropertyModel.FileCategory
                    }

                    ActionGroup.group: groupFilterActionGroup
                }
                Kirigami.Action {
                    checkable: true
                    text: i18nc("@action:inmenu audio", "Audio")

                    onTriggered: {
                        var model = root.m_mpv.visibleFilterProxyModel.availableGroupProxyModel()
                        model.filterCategory = PlaylistSortPropertyModel.AudioCategory
                    }

                    ActionGroup.group: groupFilterActionGroup
                }
                Kirigami.Action {
                    checkable: true
                    text: i18nc("@action:inmenu video", "Video")

                    onTriggered: {
                        const model = root.m_mpv.visibleFilterProxyModel.availableGroupProxyModel()
                        model.filterCategory = PlaylistSortPropertyModel.VideoCategory
                    }

                    ActionGroup.group: groupFilterActionGroup
                }
            }
        }

        RowLayout {
            Layout.margins: Kirigami.Units.largeSpacing
            CheckBox {
                text: i18nc("@action:inmenu", "Show Sections")
                checked: root.m_mpv.visibleFilterProxyModel.showSections

                onCheckedChanged: {
                    root.m_mpv.visibleFilterProxyModel.showSections = checked
                }

            }
            ToolTipButton {
                toolTipText: i18nc("@info:tooltip",
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

            ListView {
                readonly property real scrollbarWidth: availableGroupScrollView.effectiveScrollBarWidth
                readonly property bool isActiveModel: false
                readonly property bool isGroup: true

                anchors.fill: parent
                model: root.m_mpv.visibleFilterProxyModel.availableGroupProxyModel()
                delegate: benchedItemDelegate

                displaced: Transition {
                    NumberAnimation {
                        properties: "y"
                        duration: Kirigami.Units.shortDuration
                    }
                }

                leftMargin: Kirigami.Units.largeSpacing
                rightMargin: Kirigami.Units.largeSpacing
                topMargin: Kirigami.Units.largeSpacing
                bottomMargin: Kirigami.Units.largeSpacing
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

                ListView {
                    id: activeGroupListView

                    readonly property real scrollbarWidth: activeGroupScrollView.effectiveScrollBarWidth
                    readonly property bool isActiveModel: true
                    readonly property bool isGroup: true

                    anchors.fill: parent
                    model: root.m_mpv.visibleFilterProxyModel.activeGroupModel()
                    delegate: benchedItemDelegate

                    displaced: Transition {
                        NumberAnimation {
                            properties: "y"
                            duration: Kirigami.Units.shortDuration
                        }
                    }

                    leftMargin: Kirigami.Units.largeSpacing
                    rightMargin: Kirigami.Units.largeSpacing
                    topMargin: Kirigami.Units.largeSpacing
                    bottomMargin: Kirigami.Units.largeSpacing
                }
            }

            ToolButton {
                icon.name: "newline"
                text: i18nc("@action:inmenu add new line button", "Add New Line")
                Layout.alignment: Qt.AlignRight

                onClicked: {
                    root.m_mpv.visibleFilterProxyModel.addToActiveGroup(PlaylistSortProxyModel.Separator)
                }
            }

        }
    }

    Component {
        id: benchedItemDelegate

        Item {
            id: itemDelegate

            required property int index
            required property string label
            required property int sort
            required property int category
            required property int order
            required property bool hideBlank
            // which listview is this delegate is in?
            readonly property bool isActiveModel: ListView.view.isActiveModel
            readonly property bool isGroup: ListView.view.isGroup

            implicitHeight: Kirigami.Units.gridUnit * 2
            width: ListView.view.width - (ListView.view.scrollbarWidth + ListView.view.leftMargin + ListView.view.rightMargin)

            Rectangle {
                id: itemDelegateBackgroundRect

                width: itemDelegate.width
                height: itemDelegate.height
                radius: Kirigami.Units.cornerRadius

                border {
                    width: itemDelegateHoverHandler.hovered ? 1 : 0
                    color: Kirigami.Theme.hoverColor
                }

                color: {
                    if (itemDelegateDragHandle.dragActive) {
                        return Kirigami.Theme.positiveBackgroundColor
                    }
                    if (itemDelegate.isActiveModel && itemDelegateHoverHandler.hovered) {
                        return Kirigami.Theme.negativeBackgroundColor
                    }
                    return "transparent"
                }

                RowLayout {
                    anchors.fill: itemDelegateBackgroundRect
                    spacing: 0

                    Kirigami.ListItemDragHandle {
                        id: itemDelegateDragHandle

                        visible: itemDelegate.isActiveModel
                        enabled: itemDelegate.isActiveModel
                        listItem: itemDelegateBackgroundRect
                        listView: itemDelegate.ListView?.view

                        onMoveRequested: function(oldIndex, newIndex) {
                            if (itemDelegate.isGroup) {
                                var model = root.m_mpv.visibleFilterProxyModel.activeGroupModel()
                            }
                            else {
                                var model = root.m_mpv.visibleFilterProxyModel.activeSortPropertiesModel()
                            }
                            model.moveSortProperty(oldIndex, newIndex)
                        }

                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                        Layout.leftMargin: Kirigami.Units.mediumSpacing
                        Layout.rightMargin: Kirigami.Units.mediumSpacing
                    }

                    Kirigami.Separator {
                        visible: itemDelegate.isActiveModel
                        Layout.fillHeight: true
                        Layout.topMargin: Kirigami.Units.largeSpacing
                        Layout.bottomMargin: Kirigami.Units.largeSpacing
                        Layout.rightMargin: Kirigami.Units.mediumSpacing
                    }

                    Kirigami.IconTitleSubtitle {
                        title: itemDelegate.label

                        icon.name: {
                            switch(itemDelegate.category) {
                                case (PlaylistSortPropertyModel.FileCategory):
                                    return "folder-documents-symbolic"
                                case (PlaylistSortPropertyModel.AudioCategory):
                                switch(itemDelegate.sort) {
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

                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.leftMargin: itemDelegate.isActiveModel ? 0 : Kirigami.Units.largeSpacing

                        TapHandler {
                            id: itemDelegateTapHandler

                            acceptedButtons: Qt.LeftButton

                            onSingleTapped: function(eventPoint, mouseButton) {
                                switch (mouseButton) {
                                case Qt.LeftButton:
                                    if (itemDelegate.isGroup) {
                                        if (itemDelegate.isActiveModel) {
                                            root.m_mpv.visibleFilterProxyModel.removeFromActiveGroup(itemDelegate.index)
                                        } else {
                                            root.m_mpv.visibleFilterProxyModel.addToActiveGroup(itemDelegate.sort)
                                        }
                                    }
                                    else {
                                        if (itemDelegate.isActiveModel) {
                                            root.m_mpv.visibleFilterProxyModel.removeFromActiveSortProperties(itemDelegate.index)
                                        } else {
                                            root.m_mpv.visibleFilterProxyModel.addToActiveSortProperties(itemDelegate.sort)
                                        }
                                    }
                                }
                            }
                        }

                        HoverHandler {
                            id: itemDelegateHoverHandler
                        }
                    }

                    Button {
                        flat: true
                        hoverEnabled: true
                        visible: itemDelegate.isActiveModel
                        icon.width: Kirigami.Units.iconSizes.small
                        icon.height: Kirigami.Units.iconSizes.small

                        icon.name: {
                            if (itemDelegate.isGroup) {
                                if (itemDelegate.hideBlank){
                                    return "password-show-off"
                                } else {
                                    return "password-show-on"
                                }
                            } else {
                                switch(itemDelegate.order){
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
                            if (itemDelegate.isGroup) {
                                root.m_mpv.visibleFilterProxyModel.setGroupHideBlank(itemDelegate.index, !itemDelegate.hideBlank)
                                return
                            } else {
                                switch(itemDelegate.order){
                                    case (PlaylistSortPropertyModel.Ascending):
                                        root.m_mpv.visibleFilterProxyModel.setSortPropertySortingOrder(itemDelegate.index, PlaylistSortPropertyModel.Descending)
                                        return
                                    case (PlaylistSortPropertyModel.Descending):
                                        root.m_mpv.visibleFilterProxyModel.setSortPropertySortingOrder(itemDelegate.index, PlaylistSortPropertyModel.SameAsPrimary)
                                        return
                                    case (PlaylistSortPropertyModel.SameAsPrimary):
                                        root.m_mpv.visibleFilterProxyModel.setSortPropertySortingOrder(itemDelegate.index, PlaylistSortPropertyModel.Ascending)
                                        return
                                    default:
                                        return
                                }
                            }
                        }

                        Layout.rightMargin: Kirigami.Units.smallSpacing
                        Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                        ToolTip.visible: hovered
                        ToolTip.delay: Kirigami.Units.shortDuration

                        ToolTip.text: {
                            if (itemDelegate.isGroup) {
                                if (itemDelegate.hideBlank) {
                                    return i18nc("@info:tooltip show blank", "Property is hidden if empty or not unique.")
                                } else {
                                    return i18nc("@info:tooltip hide blank", "Property is shown, if empty a placeholder will be shown.")
                                }
                            } else {
                                switch(itemDelegate.order){
                                    case (PlaylistSortPropertyModel.Ascending):
                                        return i18nc("@info:tooltip group order", "Sort ascending")
                                    case (PlaylistSortPropertyModel.Descending):
                                        return i18nc("@info:tooltip group order", "Sort descending")
                                    case (PlaylistSortPropertyModel.SameAsPrimary):
                                        return i18nc("@info:tooltip group order", "Same as primary sort order")
                                    default:
                                        return ""
                                }
                            }
                        }
                    }
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
