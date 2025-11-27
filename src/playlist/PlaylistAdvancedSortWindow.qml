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

    width: 500
    height: 500
    minimumWidth: 250
    minimumHeight: 250
    title: i18nc("@title:configure sort window", "Advanced Sort and Group By")
    transientParent: null

    Shortcut {
        sequence: "Esc"
        onActivated: root.close()
    }

    header: ToolBar {
        width: root.width

        RowLayout {
            anchors.fill: parent
            spacing: 0

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
                    },
                    Kirigami.Action {
                        text: {
                            switch(root.m_mpv.visibleFilterProxyModel.sortRole){
                                case (PlaylistSortProxyModel.None):
                                    return actionSource.text
                                case (PlaylistSortProxyModel.FileName):
                                    return actionFileName.text
                                case (PlaylistSortProxyModel.Duration):
                                    return actionDuration.text
                                case (PlaylistSortProxyModel.Date):
                                    return actionDate.text
                                case (PlaylistSortProxyModel.FileSize):
                                    return actionFileSize.text
                                case (PlaylistSortProxyModel.Title):
                                    return actionTitle.text
                                case (PlaylistSortProxyModel.TrackNo):
                                    return actionTrackNo.text
                                case (PlaylistSortProxyModel.SampleRate):
                                    return actionSampleRate.text
                                case (PlaylistSortProxyModel.Bitrate):
                                    return actionBitrate.text
                                default:
                                    return ""
                            }
                        }
                        displayHint: Kirigami.DisplayHint.KeepVisible

                        Kirigami.Action {
                            id: actionSource

                            autoExclusive: true
                            checkable: true
                            checked: root.m_mpv.visibleFilterProxyModel.sortRole === PlaylistSortProxyModel.None
                            text: i18nc("@action:inmenu", "None")

                            onTriggered: {
                                root.m_mpv.visibleFilterProxyModel.sortRole = PlaylistSortProxyModel.None
                            }
                        }
                        Kirigami.Action {
                            id: actionFileName

                            autoExclusive: true
                            checkable: true
                            checked: root.m_mpv.visibleFilterProxyModel.sortRole === PlaylistSortProxyModel.FileName
                            text: i18nc("@action:inmenu", "File Name")

                            onTriggered: {
                                root.m_mpv.visibleFilterProxyModel.sortRole = PlaylistSortProxyModel.FileName
                            }
                        }
                        Kirigami.Action {
                            id: actionTitle

                            autoExclusive: true
                            checkable: true
                            checked: root.m_mpv.visibleFilterProxyModel.sortRole === PlaylistSortProxyModel.Title
                            text: i18nc("@action:inmenu", "Title")

                            onTriggered: {
                                root.m_mpv.visibleFilterProxyModel.sortRole = PlaylistSortProxyModel.Title
                            }
                        }
                        Kirigami.Action {
                            id: actionDuration

                            autoExclusive: true
                            checkable: true
                            checked: root.m_mpv.visibleFilterProxyModel.sortRole === PlaylistSortProxyModel.Duration
                            text: i18nc("@action:inmenu", "Duration")

                            onTriggered: {
                                root.m_mpv.visibleFilterProxyModel.sortRole = PlaylistSortProxyModel.Duration
                            }
                        }
                        Kirigami.Action {
                            id: actionDate

                            autoExclusive: true
                            checkable: true
                            checked: root.m_mpv.visibleFilterProxyModel.sortRole === PlaylistSortProxyModel.Date
                            text: i18nc("@action:inmenu", "Modified Date")

                            onTriggered: {
                                root.m_mpv.visibleFilterProxyModel.sortRole = PlaylistSortProxyModel.Date
                            }
                        }
                        Kirigami.Action {
                            id: actionFileSize

                            autoExclusive: true
                            checkable: true
                            checked: root.m_mpv.visibleFilterProxyModel.sortRole === PlaylistSortProxyModel.FileSize
                            text: i18nc("@action:inmenu", "File Size")

                            onTriggered: {
                                root.m_mpv.visibleFilterProxyModel.sortRole = PlaylistSortProxyModel.FileSize
                            }
                        }
                        Kirigami.Action {
                            id: actionTrackNo

                            autoExclusive: true
                            checkable: true
                            checked: root.m_mpv.visibleFilterProxyModel.sortRole === PlaylistSortProxyModel.TrackNo
                            text: i18nc("@action:inmenu", "Track No")

                            onTriggered: {
                                root.m_mpv.visibleFilterProxyModel.sortRole = PlaylistSortProxyModel.TrackNo
                            }
                        }
                        Kirigami.Action {
                            id: actionSampleRate

                            autoExclusive: true
                            checkable: true
                            checked: root.m_mpv.visibleFilterProxyModel.sortRole === PlaylistSortProxyModel.SampleRate
                            text: i18nc("@action:inmenu", "Sample Rate")

                            onTriggered: {
                                root.m_mpv.visibleFilterProxyModel.sortRole = PlaylistSortProxyModel.SampleRate
                            }
                        }
                        Kirigami.Action {
                            id: actionBitrate

                            autoExclusive: true
                            checkable: true
                            checked: root.m_mpv.visibleFilterProxyModel.sortRole === PlaylistSortProxyModel.Bitrate
                            text: i18nc("@action:inmenu", "Bitrate")

                            onTriggered: {
                                root.m_mpv.visibleFilterProxyModel.sortRole = PlaylistSortProxyModel.Bitrate
                            }
                        }
                    }
                ]
            }
        }
    }

    footer: ToolBar {
        width: root.width

        RowLayout {
            anchors.fill: parent
            spacing: 0

            ToolTipButton {
                toolTipText: i18nc("@info:tooltip",
                                   "Playlist items will be grouped by the active <br>" +
                                   "properties in the provided order and each group <br>" +
                                   "is sorted within by the primary sort configuration set above.")
                toolTipWidth: 450
                Layout.preferredHeight: Kirigami.Units.iconSizes.medium
                Layout.alignment: Qt.AlignLeft
            }

            Kirigami.ActionToolBar {
                alignment: Qt.AlignRight
                Layout.margins: Kirigami.Units.largeSpacing
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignRight

                actions: [
                    Kirigami.Action {
                        id: addNewLine

                        displayHint: Kirigami.DisplayHint.KeepVisible
                        icon.name: "newline"
                        text: i18nc("@sort:group add new line button", "Add New Line")

                        onTriggered: {
                            root.m_mpv.visibleFilterProxyModel.addToActiveGroup(PlaylistSortProxyModel.Separator)
                        }
                    }
                ]
            }
        }
    }

    GridLayout {
        columns: 2
        anchors.fill: parent
        anchors.margins: Kirigami.Units.largeSpacing

        Label {
            text: i18nc("@label availabe grouping properties", "Available Groups")
            elide: Text.ElideLeft
            Layout.alignment: Qt.AlignLeft
        }

        Item { Layout.preferredWidth: 1; Layout.preferredHeight: 1 }

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
                    id: filterAction

                    text: i18nc("@sort:filter", "Filter")

                    icon {
                        name: "view-filter-symbolic"
                        width: Kirigami.Units.iconSizes.medium
                        height: Kirigami.Units.iconSizes.medium
                    }

                    onTriggered: {
                        filterMenu.open()
                    }
                }
            ]

            Menu {
                id: filterMenu

                x: availableGroupSearch.width - Kirigami.Units.iconSizes.medium
                y: availableGroupSearch.height
                title: i18nc("@sort:filter", "Filter")

                ActionGroup { id: filterActionGroup }

                Kirigami.Action {
                    checked: true
                    checkable: true
                    text: i18nc("@sort:filter all", "All")

                    onTriggered: {
                        var model = root.m_mpv.visibleFilterProxyModel.availableGroupProxyModel()
                        model.filterCategory = PlaylistGroupPropertyModel.All
                    }

                    ActionGroup.group: filterActionGroup
                }
                Kirigami.Action {
                    checkable: true
                    text: i18nc("@sort:filter file", "File")

                    onTriggered: {
                        var model = root.m_mpv.visibleFilterProxyModel.availableGroupProxyModel()
                        model.filterCategory = PlaylistGroupPropertyModel.FileCategory
                    }

                    ActionGroup.group: filterActionGroup
                }
                Kirigami.Action {
                    checkable: true
                    text: i18nc("@sort:filter audio", "Audio")

                    onTriggered: {
                        var model = root.m_mpv.visibleFilterProxyModel.availableGroupProxyModel()
                        model.filterCategory = PlaylistGroupPropertyModel.AudioCategory
                    }

                    ActionGroup.group: filterActionGroup
                }
                Kirigami.Action {
                    checkable: true
                    text: i18nc("@sort:filter video", "Video")

                    onTriggered: {
                        const model = root.m_mpv.visibleFilterProxyModel.availableGroupProxyModel()
                        model.filterCategory = PlaylistGroupPropertyModel.VideoCategory
                    }

                    ActionGroup.group: filterActionGroup
                }
            }
        }

        Label {
            text: i18nc("@label active grouping properties", "Active Groups:")
            elide: Text.ElideLeft
            Layout.alignment: Qt.AlignLeft
            Layout.leftMargin: Kirigami.Units.largeSpacing
        }

        ScrollView {
            id: availableScrollView

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
                id: availableListView

                readonly property real scrollbarWidth: availableScrollView.effectiveScrollBarWidth
                readonly property bool isActiveGroup: false

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

        ScrollView {
            id: activeScrollView

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

                readonly property real scrollbarWidth: activeScrollView.effectiveScrollBarWidth
                readonly property bool isActiveGroup: true

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
    }

    Component {
        id: benchedItemDelegate

        Item {
            id: itemDelegate

            required property int index
            required property string label
            required property int group
            required property int category
            required property int display
            required property int order
            // which listview is this delegate is in?
            readonly property bool isActiveGroup: ListView.view.isActiveGroup
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
                    if (itemDelegate.isActiveGroup && itemDelegateHoverHandler.hovered) {
                        return Kirigami.Theme.negativeBackgroundColor
                    }
                    return "transparent"
                }

                RowLayout {
                    anchors.fill: itemDelegateBackgroundRect
                    spacing: 0

                    Kirigami.ListItemDragHandle {
                        id: itemDelegateDragHandle

                        visible: itemDelegate.isActiveGroup
                        enabled: itemDelegate.isActiveGroup
                        listItem: itemDelegateBackgroundRect
                        listView: itemDelegate.ListView?.view

                        onMoveRequested: function(oldIndex, newIndex) {
                            var model = root.m_mpv.visibleFilterProxyModel.activeGroupModel()
                            model.moveGroupProperty(oldIndex, newIndex)
                        }

                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                        Layout.leftMargin: Kirigami.Units.mediumSpacing
                        Layout.rightMargin: Kirigami.Units.mediumSpacing
                    }

                    Kirigami.Separator {
                        visible: itemDelegate.isActiveGroup
                        Layout.fillHeight: true
                        Layout.topMargin: Kirigami.Units.largeSpacing
                        Layout.bottomMargin: Kirigami.Units.largeSpacing
                        Layout.rightMargin: Kirigami.Units.mediumSpacing
                    }

                    Kirigami.IconTitleSubtitle {
                        title: itemDelegate.label

                        icon.name: {
                            switch(itemDelegate.category) {
                                case (PlaylistGroupPropertyModel.FileCategory):
                                    return "folder-documents-symbolic"
                                case (PlaylistGroupPropertyModel.AudioCategory):
                                switch(itemDelegate.group) {
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
                                case (PlaylistGroupPropertyModel.VideoCategory):
                                    return "folder-video-symbolic"
                                case (PlaylistGroupPropertyModel.SeparatorCategory):
                                    return ""
                                default:
                                    return ""
                            }
                        }

                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.leftMargin: itemDelegate.isActiveGroup ? 0 : Kirigami.Units.largeSpacing

                        TapHandler {
                            id: itemDelegateTapHandler

                            acceptedButtons: Qt.LeftButton

                            onSingleTapped: function(eventPoint, mouseButton) {
                                switch (mouseButton) {
                                case Qt.LeftButton:
                                    if (itemDelegate.isActiveGroup) {
                                        root.m_mpv.visibleFilterProxyModel.removeFromActiveGroup(itemDelegate.index)
                                    } else {
                                        root.m_mpv.visibleFilterProxyModel.addToActiveGroup(itemDelegate.group)
                                    }
                                }
                            }
                        }

                        HoverHandler {
                            id: itemDelegateHoverHandler
                        }
                    }

                    Button {
                        id: sortOrderButton

                        flat: true
                        hoverEnabled: true
                        visible: itemDelegate.isActiveGroup && itemDelegate.category != PlaylistGroupPropertyModel.SeparatorCategory
                        icon.width: Kirigami.Units.iconSizes.small
                        icon.height: Kirigami.Units.iconSizes.small

                        icon.name: {
                            switch(itemDelegate.order){
                                case (PlaylistGroupPropertyModel.Ascending):
                                    return "view-sort-ascending-name"
                                case (PlaylistGroupPropertyModel.Descending):
                                    return "view-sort-descending-name"
                                case (PlaylistGroupPropertyModel.SameAsPrimary):
                                    return "view-sort"
                                default:
                                    return ""
                            }
                        }

                        onClicked: {
                            switch(itemDelegate.order){
                                case (PlaylistGroupPropertyModel.Ascending):
                                    root.m_mpv.visibleFilterProxyModel.setGroupSortOrder(itemDelegate.index, PlaylistGroupPropertyModel.Descending)
                                    return
                                case (PlaylistGroupPropertyModel.Descending):
                                    root.m_mpv.visibleFilterProxyModel.setGroupSortOrder(itemDelegate.index, PlaylistGroupPropertyModel.SameAsPrimary)
                                    return
                                case (PlaylistGroupPropertyModel.SameAsPrimary):
                                    root.m_mpv.visibleFilterProxyModel.setGroupSortOrder(itemDelegate.index, PlaylistGroupPropertyModel.Ascending)
                                    return
                                default:
                                    return
                            }
                        }

                        Layout.rightMargin: Kirigami.Units.smallSpacing
                        Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                        ToolTip.visible: hovered
                        ToolTip.delay: Kirigami.Units.shortDuration

                        ToolTip.text: {
                            switch(itemDelegate.order){
                                case (PlaylistGroupPropertyModel.Ascending):
                                    return i18nc("@info:tooltip group order", "Sort ascending")
                                case (PlaylistGroupPropertyModel.Descending):
                                    return i18nc("@info:tooltip group order", "Sort descending")
                                case (PlaylistGroupPropertyModel.SameAsPrimary):
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

    function calculatePostLayout() {
        // effectiveScrollBarWidth is always 0 at load time because of the Loader.
        // We set vertical scroll policy to ScrollBar.AlwaysOff initially then
        // set them back to ScrollBar.AsNeeded after the item is loaded via this function
        availableScrollView.ScrollBar.vertical.policy = ScrollBar.AsNeeded
        activeScrollView.ScrollBar.vertical.policy = ScrollBar.AsNeeded
    }
}
