/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 * SPDX-FileCopyrightText: 2025 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import QtQuick.Dialogs

import org.kde.kirigami as Kirigami
import org.kde.haruna
import org.kde.haruna.playlist
import org.kde.haruna.utilities
import org.kde.haruna.settings
import org.kde.haruna.youtube

Page {
    id: root

    required property MpvVideo m_mpv
    required property Loader m_advancedSortWindowLoader

    property bool isSmallWindowSize: Window.window.width < 600
    property int buttonSize: isSmallWindowSize ? Kirigami.Units.iconSizes.small : Kirigami.Units.iconSizes.smallMedium
    property alias scrollPositionTimer: scrollPositionTimer
    property alias playlistView: playlistView
    property real customWidth: PlaylistSettings.playlistWidth
    property real fsScale: Window.window.isFullScreen() && PlaylistSettings.bigFontFullscreen ? 1.36 : 1

    width: limitWidth(customWidth) * fsScale

    function limitWidth(pWidth) {
        if (PlaylistSettings.style === "compact") {
            return 380
        } else {
            return Math.min(Math.max(pWidth, 260), Window.window.width - 50)
        }
    }

    x: PlaylistSettings.position === "right" ? Window.window.width : -width
    y: 0
    padding: 0
    state: PlaylistSettings.rememberState
           ? (PlaylistSettings.visible ? "visible" : "hidden")
           : "hidden"

    onStateChanged: {
        PlaylistSettings.visible = state === "visible" ? true : false
        PlaylistSettings.save()
        if (state === "hidden") {
            contextMenuLoader.active = false
        }
    }

    header: ToolBar {
        id: toolbar

        width: parent.width
        visible: PlaylistSettings.showToolbar
        height: visible ? implicitHeight : 0

        bottomPadding: 0

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            Kirigami.ActionToolBar {
                ActionGroup { id: sortOrderGroup }
                ActionGroup { id: sortPresetGroup }
                actions: [
                    Kirigami.Action {
                        text: i18nc("@action:button", "Search")
                        icon.name: "search"
                        icon.width: root.buttonSize
                        icon.height: root.buttonSize
                        displayHint: Kirigami.DisplayHint.IconOnly
                        displayComponent: Kirigami.SearchField {
                            id: searchComponent
                            delaySearch: true

                            onTextChanged: {
                                root.m_mpv.visibleFilterProxyModel.searchText = text
                                playlistView.positionViewAtIndex(0, ListView.Beginning)
                            }

                            Component.onCompleted: {
                                text = root.m_mpv.visibleFilterProxyModel.searchText
                            }
                        }
                    },
                    Kirigami.Action {
                        text: i18nc("@action:button", "Add…")
                        displayHint: root.isSmallWindowSize
                                     ? Kirigami.DisplayHint.IconOnly
                                     : Kirigami.DisplayHint.NoPreference
                        icon.name: "list-add"
                        icon.width: root.buttonSize
                        icon.height: root.buttonSize
                        Kirigami.Action {
                            text: i18nc("@action:button", "Files")
                            onTriggered: {
                                fileDialog.fileType = "video"
                                fileDialog.fileMode = FileDialog.OpenFiles
                                fileDialog.open()
                            }
                        }
                        Kirigami.Action {
                            text: i18nc("@action:button", "URL")
                            onTriggered: {
                                if (addUrlPopup.opened) {
                                    addUrlPopup.close()
                                } else {
                                    addUrlPopup.open()
                                }
                            }
                        }
                        Kirigami.Action {
                            text: i18nc("@action:button", "Playlist")
                            onTriggered: {
                                fileDialog.fileType = "playlist"
                                fileDialog.fileMode = FileDialog.OpenFile
                                fileDialog.open()
                            }
                        }
                    },
                    Kirigami.Action {
                        text: i18nc("@action:button", "Sort")
                        displayHint: root.isSmallWindowSize
                                     ? Kirigami.DisplayHint.IconOnly
                                     : Kirigami.DisplayHint.NoPreference
                        icon.name: "view-sort"
                        icon.width: root.buttonSize
                        icon.height: root.buttonSize

                        Kirigami.Action {
                            text: i18nc("@action:button", "Ascending")
                            checkable: true
                            checked: root.m_mpv.visibleFilterProxyModel.sortOrder === Qt.AscendingOrder

                            icon {
                                name: "view-sort-ascending-name"
                                width: root.buttonSize
                                height: root.buttonSize
                            }

                            onTriggered: {
                                root.m_mpv.visibleFilterProxyModel.sortOrder = Qt.AscendingOrder
                            }

                            ActionGroup.group: sortOrderGroup
                        }
                        Kirigami.Action {
                            text: i18nc("@action:button", "Descending")
                            checkable: true
                            checked: root.m_mpv.visibleFilterProxyModel.sortOrder === Qt.DescendingOrder

                            icon {
                                name: "view-sort-descending-name"
                                width: root.buttonSize
                                height: root.buttonSize
                            }
                            onTriggered: {
                                root.m_mpv.visibleFilterProxyModel.sortOrder = Qt.DescendingOrder
                            }

                            ActionGroup.group: sortOrderGroup
                        }

                        Kirigami.Action {
                            separator: true
                        }

                        Kirigami.Action {
                            checkable: true
                            checked: root.m_mpv.visibleFilterProxyModel.sortPreset === PlaylistSortProxyModel.None
                            text: i18nc("@action:button", "None")

                            onTriggered: {
                                root.m_mpv.visibleFilterProxyModel.sortPreset = PlaylistSortProxyModel.None
                            }
                            ActionGroup.group: sortPresetGroup
                        }
                        Kirigami.Action {
                            checkable: true
                            checked: root.m_mpv.visibleFilterProxyModel.sortPreset === PlaylistSortProxyModel.FileName
                            text: i18nc("@action:button", "File Name")

                            onTriggered: {
                                root.m_mpv.visibleFilterProxyModel.sortPreset = PlaylistSortProxyModel.FileName
                            }
                            ActionGroup.group: sortPresetGroup
                        }
                        Kirigami.Action {
                            checkable: true
                            checked: root.m_mpv.visibleFilterProxyModel.sortPreset === PlaylistSortProxyModel.Title
                            text: i18nc("@action:button", "Title")

                            onTriggered: {
                                root.m_mpv.visibleFilterProxyModel.sortPreset = PlaylistSortProxyModel.Title
                            }
                            ActionGroup.group: sortPresetGroup
                        }
                        Kirigami.Action {
                            checkable: true
                            checked: root.m_mpv.visibleFilterProxyModel.sortPreset === PlaylistSortProxyModel.Duration
                            text: i18nc("@action:button", "Duration")

                            onTriggered: {
                                root.m_mpv.visibleFilterProxyModel.sortPreset = PlaylistSortProxyModel.Duration
                            }
                            ActionGroup.group: sortPresetGroup
                        }
                        Kirigami.Action {
                            checkable: true
                            checked: root.m_mpv.visibleFilterProxyModel.sortPreset === PlaylistSortProxyModel.Date
                            text: i18nc("@action:button", "Modified Date")

                            onTriggered: {
                                root.m_mpv.visibleFilterProxyModel.sortPreset = PlaylistSortProxyModel.Date
                            }
                            ActionGroup.group: sortPresetGroup
                        }
                        Kirigami.Action {
                            checkable: true
                            checked: root.m_mpv.visibleFilterProxyModel.sortPreset === PlaylistSortProxyModel.FileSize
                            text: i18nc("@action:button", "File Size")

                            onTriggered: {
                                root.m_mpv.visibleFilterProxyModel.sortPreset = PlaylistSortProxyModel.FileSize
                            }
                            ActionGroup.group: sortPresetGroup
                        }
                        Kirigami.Action {
                            checkable: true
                            checked: root.m_mpv.visibleFilterProxyModel.sortPreset === PlaylistSortProxyModel.TrackNo
                            text: i18nc("@action:button", "Track No")

                            onTriggered: {
                                root.m_mpv.visibleFilterProxyModel.sortPreset = PlaylistSortProxyModel.TrackNo
                            }
                            ActionGroup.group: sortPresetGroup
                        }
                        Kirigami.Action {
                            checkable: true
                            checked: root.m_mpv.visibleFilterProxyModel.sortPreset === PlaylistSortProxyModel.SampleRate
                            text: i18nc("@action:button", "Sample Rate")

                            onTriggered: {
                                root.m_mpv.visibleFilterProxyModel.sortPreset = PlaylistSortProxyModel.SampleRate
                            }
                            ActionGroup.group: sortPresetGroup
                        }
                        Kirigami.Action {
                            checkable: true
                            checked: root.m_mpv.visibleFilterProxyModel.sortPreset === PlaylistSortProxyModel.Bitrate
                            text: i18nc("@action:button", "Bitrate")

                            onTriggered: {
                                root.m_mpv.visibleFilterProxyModel.sortPreset = PlaylistSortProxyModel.Bitrate
                            }
                            ActionGroup.group: sortPresetGroup
                        }

                        Kirigami.Action {
                            separator: true
                        }

                        Kirigami.Action {
                            checkable: true
                            checked: root.m_mpv.visibleFilterProxyModel.sortPreset === PlaylistSortProxyModel.Custom
                            text: i18nc("@action:button", "Custom…")

                            onTriggered: {
                                root.m_advancedSortWindowLoader.openSortWindow()
                                root.m_mpv.visibleFilterProxyModel.itemsSorted()
                            }
                            ActionGroup.group: sortPresetGroup
                        }
                    },

                    Kirigami.Action {
                        text: i18nc("@action:button", "Playback")
                        icon.name: "media-playback-start"
                        icon.width: root.buttonSize
                        icon.height: root.buttonSize
                        displayHint: Kirigami.DisplayHint.KeepVisible
                        Kirigami.Action {
                            text: i18nc("@action:button", "Repeat playlist")
                            icon.name: "media-playlist-repeat"
                            autoExclusive: true
                            checkable: true
                            checked: PlaylistSettings.playbackBehavior === "RepeatPlaylist"
                            onTriggered: {
                                PlaylistSettings.playbackBehavior = "RepeatPlaylist"
                                PlaylistSettings.save()
                            }
                        }
                        Kirigami.Action {
                            text: i18nc("@action:button", "Stop after last item")
                            autoExclusive: true
                            checkable: true
                            checked: PlaylistSettings.playbackBehavior === "StopAfterLast"
                            onTriggered: {
                                PlaylistSettings.playbackBehavior = "StopAfterLast"
                                PlaylistSettings.save()
                            }
                        }
                        Kirigami.Action {
                            text: i18nc("@action:button", "Repeat item")
                            autoExclusive: true
                            checkable: true
                            checked: PlaylistSettings.playbackBehavior === "RepeatItem"
                            icon.name: "media-playlist-repeat-song"
                            onTriggered: {
                                PlaylistSettings.playbackBehavior = "RepeatItem"
                                PlaylistSettings.save()
                            }
                        }
                        Kirigami.Action {
                            text: i18nc("@action:button", "Stop after item")
                            autoExclusive: true
                            checkable: true
                            checked: PlaylistSettings.playbackBehavior === "StopAfterItem"
                            onTriggered: {
                                PlaylistSettings.playbackBehavior = "StopAfterItem"
                                PlaylistSettings.save()
                            }
                        }
                        Kirigami.Action {
                            text: i18nc("@action:button", "Random Playback")
                            checkable: true
                            enabled: ["StopAfterLast", "RepeatPlaylist"].includes(PlaylistSettings.playbackBehavior)
                            checked: PlaylistSettings.randomPlayback
                            icon.name: "randomize"
                            onTriggered: {
                                PlaylistSettings.randomPlayback = checked
                                PlaylistSettings.save()
                            }
                        }
                    },
                    Kirigami.Action {
                        text: i18nc("@action:button", "Clear")
                        icon.name: "edit-clear-all"
                        displayHint: Kirigami.DisplayHint.AlwaysHide
                        onTriggered: {
                            root.m_mpv.visibleFilterProxyModel.clear()
                        }
                    },
                    Kirigami.Action {
                        text: i18nc("@action:button", "Save As")
                        icon.name: "document-save-as"
                        displayHint: Kirigami.DisplayHint.AlwaysHide
                        onTriggered: {
                            fileDialog.fileType = "playlist"
                            fileDialog.fileMode = FileDialog.SaveFile
                            fileDialog.open()
                        }
                    }
                ]
            }

            RowLayout {
            TabBar {
                id: playlistTabView

                Layout.alignment: Qt.AlignLeft | Qt.AlignBottom
                Layout.fillWidth: true
                clip: true

                onCurrentIndexChanged: {
                    root.m_mpv.playlists.visibleIndex = currentIndex
                }

                Repeater {
                    model: root.m_mpv.playlists
                    delegate: PlaylistTabDelegate {
                        m_mpv: root.m_mpv
                    }
                }

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

                // This connection is only necessary for the startup. If the last played item is inside
                // an internal tab, then the TabBar should set its current index to that tab.
                Connections {
                    target: root.m_mpv.playlists
                    function onVisibleIndexChanged() {
                        playlistTabView.currentIndex = root.m_mpv.playlists.visibleIndex
                    }
                }
                }
                ToolButton {
                    icon.name: "list-add"
                    onClicked: {
                        if (addPlaylistPopup.opened) {
                            addPlaylistPopup.close()
                        } else {
                            addPlaylistPopup.open()
                        }
                    }

                    ToolTip {
                        text: i18nc("@action:button", "Add new playlist")
                    }
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
                        text: i18nc("@action:inmenu", "Rename")
                        icon.name: "edit-rename"
                        visible: tabMenuLoader.row > 0
                        onClicked: root.m_mpv.playlists.renamePlaylist(tabMenuLoader.row)
                    }
                    MenuItem {
                        text: i18nc("@action:inmenu", "Remove")
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
                            onClicked: playlistTabView.setCurrentIndex(index)
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
                    contextMenu.popup()
                }
            }
        }
    }

    InputPopup {
        id: addUrlPopup

        x: Kirigami.Units.largeSpacing
        y: Kirigami.Units.largeSpacing
        width: toolbar.width - Kirigami.Units.largeSpacing * 2
        buttonText: i18nc("@action:button", "Add")
        warningText: youtube.hasYoutubeDl()
                     ? ""
                     : i18nc("@info", "Neither <a href=\"https://github.com/yt-dlp/yt-dlp\">yt-dlp</a> nor <a href=\"https://github.com/ytdl-org/youtube-dl\">youtube-dl</a> was found.")

        onSubmitted: function(url) {
            root.m_mpv.visibleFilterProxyModel.addItem(url, PlaylistModel.Append)
        }

        YouTube {
            id: youtube
        }
    }

    InputPopup {
        id: addPlaylistPopup

        x: Kirigami.Units.largeSpacing
        y: Kirigami.Units.largeSpacing
        width: toolbar.width - Kirigami.Units.largeSpacing * 2
        placeholderText: i18nc("@placeholder", "playlist name")
        buttonText: i18nc("@action:button", "Add")

        onSubmitted: function(plName) {
            root.m_mpv.playlists.createNewPlaylist(plName)
        }
    }

    component ResizeHandler: Item {
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton
            cursorShape: Qt.SizeHorCursor

            drag {
                target: parent
                axis: Drag.XAxis
                threshold: 0
            }

            onPositionChanged: {
                if (!drag.active) {
                    return
                }

                if (PlaylistSettings.position === "right") {
                    let mX = root.m_mpv.mapFromItem(this, mouseX, mouseY).x
                    var w = root.limitWidth(Window.window.width - mX)
                } else {
                    let mX = playlistView.mapFromItem(this, mouseX, mouseY).x
                    var w = root.limitWidth(mX)
                }
                root.customWidth = w / root.fsScale
            }

            onReleased: {
                PlaylistSettings.playlistWidth = root.customWidth
                PlaylistSettings.save()
            }
        }
    }

    Rectangle {
        Rectangle {
            id: playlistEdgeBorder

            x: PlaylistSettings.position === "right" ? 0 : parent.width - width
            y: -root.implicitHeaderHeight
            z: 30
            width: 1
            height: root.height
            color: Kirigami.Theme.backgroundColor

            ResizeHandler {
                anchors.horizontalCenter: parent.horizontalCenter
                width: 8
                height: parent.height
            }

            Rectangle {
                id: dragHandle

                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                width: 5
                height: 50
                color: Kirigami.Theme.alternateBackgroundColor
                radius: Kirigami.Units.cornerRadius
                border {
                    width: 1
                    color: Kirigami.Theme.backgroundColor
                }

                ResizeHandler {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: 18
                    height: parent.height
                }
            }
        }

        anchors.fill: parent
        color: Kirigami.Theme.backgroundColor

        DropArea {
            id: playlistDropArea

            anchors.fill: playlistScrollView
            keys: ["text/uri-list"]

            onDropped: drop => {
                if (!containsDrag) {
                    return
                }
                root.m_mpv.visibleFilterProxyModel.addFilesAndFolders(drop.urls, PlaylistModel.Append)
            }
        }

        ScrollView {
            id: playlistScrollView

            z: 20
            anchors.fill: parent
            anchors {
                leftMargin: playlistEdgeBorder.width
                rightMargin: playlistEdgeBorder.width
            }

            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

            ListView {
                id: playlistView

                // set bottomMargin so that the footer doesn't block playlist items
                bottomMargin: GeneralSettings.footerStyle === "default" ? 0 : 100

                model: root.m_mpv.visibleFilterProxyModel
                onModelChanged: {
                    Qt.callLater(root.m_mpv.visibleFilterProxyModel.refreshData)
                }

                reuseItems: true
                spacing: 1
                currentIndex: root.m_mpv.visibleFilterProxyModel.getPlayingItem()

                moveDisplaced: Transition {
                    NumberAnimation {
                        properties: "y"
                        duration: Kirigami.Units.shortDuration
                    }
                }

                delegate: {
                    switch (PlaylistSettings.style) {
                    case "default":
                        playlistItemSimple
                        break
                    case "withThumbnails":
                        playlistItemWithThumbnail
                        break
                    case "compact":
                        playlistItemCompact
                        break
                    }
                }

                section {
                    property: "section"
                    delegate: PlaylistSectionDelegate {
                        model: root.m_mpv.visibleFilterProxyModel
                    }
                }

                MouseArea {
                    z: -1
                    anchors.fill: playlistView
                    acceptedButtons: Qt.MiddleButton | Qt.RightButton
                    onClicked: function(mouse) {
                        switch (mouse.button) {
                        case Qt.MiddleButton:
                            const index = root.m_mpv.visibleFilterProxyModel.getPlayingItem()
                            playlistView.positionViewAtIndex(index, ListView.Beginning)
                            break
                        case Qt.RightButton:
                            // Open the menu just to present options like selection
                            contextMenuLoader.open(null)
                            break
                        }
                    }
                }

                function openContextMenu(item) {
                    item = item as PlaylistItemDelegate
                    if (!item) {
                        return
                    }
                    contextMenuLoader.open(item)
                }
            }
        }

        Loader {
            id: contextMenuLoader

            property int row: -1
            property bool isLocal: false

            active: false
            asynchronous: true
            sourceComponent: Menu {

                MenuItem {
                    text: i18nc("@action:inmenu", "Open Containing Folder")
                    icon.name: "folder"
                    visible: contextMenuLoader.isLocal && contextMenuLoader.row != -1
                    onClicked: root.m_mpv.visibleFilterProxyModel.highlightInFileManager(contextMenuLoader.row)
                }
                MenuItem {
                    text: i18nc("@action:inmenu", "Open in Browser")
                    icon.name: "link"
                    visible: !contextMenuLoader.isLocal && contextMenuLoader.row != -1
                    onClicked: {
                        const modelIndex = root.m_mpv.visibleFilterProxyModel.index(contextMenuLoader.row, 0)
                        Qt.openUrlExternally(modelIndex.data(PlaylistModel.PathRole))
                    }
                }
                MenuItem {
                    text: i18nc("@action:inmenu", "Copy Name")
                    onClicked: root.m_mpv.visibleFilterProxyModel.copyFileName(contextMenuLoader.row)
                    visible: contextMenuLoader.row != -1
                }
                MenuItem {
                    text: contextMenuLoader.isLocal
                          ? i18nc("@action:inmenu", "Copy Path")
                          : i18nc("@action:inmenu", "Copy URL")
                    onClicked: root.m_mpv.visibleFilterProxyModel.copyFilePath(contextMenuLoader.row)
                    visible: contextMenuLoader.row != -1
                }
                MenuSeparator {
                    visible: contextMenuLoader.row != -1
                }

                // Selection manipulators
                MenuItem {
                    text: i18nc("@action:inmenu", "Select All")
                    onClicked: root.m_mpv.visibleFilterProxyModel.selectItem(0, PlaylistFilterProxyModel.All)
                }
                MenuItem {
                    text: i18nc("@action:inmenu", "Deselect All")
                    onClicked: root.m_mpv.visibleFilterProxyModel.selectItem(0, PlaylistFilterProxyModel.Clear)
                }
                MenuItem {
                    text: i18nc("@action:inmenu", "Invert Selection")
                    onClicked: root.m_mpv.visibleFilterProxyModel.selectItem(0, PlaylistFilterProxyModel.Invert)
                }
                MenuSeparator {
                }
                MenuItem {
                    text: i18nc("@action:inmenu", "Remove from Playlist")
                    icon.name: "remove"
                    onClicked: root.m_mpv.visibleFilterProxyModel.removeItem(contextMenuLoader.row)
                    visible: root.m_mpv.visibleFilterProxyModel.selectionCount === 1 && contextMenuLoader.row != -1
                }
                MenuItem {
                    text: i18nc("@action:inmenu", "Remove Selected from Playlist")
                    icon.name: "remove"
                    onClicked: root.m_mpv.visibleFilterProxyModel.removeItems()
                    visible: root.m_mpv.visibleFilterProxyModel.selectionCount > 1 && contextMenuLoader.row != -1
                }
                MenuItem {
                    text: i18nc("@action:inmenu", "Rename")
                    icon.name: "edit-rename"
                    visible: contextMenuLoader.isLocal && contextMenuLoader.row != -1
                    onClicked: root.m_mpv.visibleFilterProxyModel.renameFile(contextMenuLoader.row)
                }
                MenuItem {
                    text: i18nc("@action:inmenu", "Scroll to Playing Item")
                    onClicked: {
                        const index = root.m_mpv.visibleFilterProxyModel.getPlayingItem()
                        playlistView.positionViewAtIndex(index, ListView.Beginning)
                    }
                }
                MenuSeparator {
                    visible: contextMenuLoader.isLocal && contextMenuLoader.row != -1
                }
                MenuItem {
                    text: i18nc("@action:inmenu", "Move File to Trash")
                    icon.name: "delete"
                    visible: contextMenuLoader.isLocal && root.m_mpv.visibleFilterProxyModel.selectionCount === 1 && contextMenuLoader.row != -1
                    onClicked: root.m_mpv.visibleFilterProxyModel.trashFile(contextMenuLoader.row)
                }
                MenuItem {
                    text: i18nc("@action:inmenu", "Move Selected Files to Trash")
                    icon.name: "delete"
                    visible: contextMenuLoader.isLocal && root.m_mpv.visibleFilterProxyModel.selectionCount > 1 && contextMenuLoader.row != -1
                    onClicked: root.m_mpv.visibleFilterProxyModel.trashFiles()
                }
            }

            function open(item: PlaylistItemDelegate) : void {
                if (!contextMenuLoader.active) {
                    contextMenuLoader.active = true
                    contextMenuLoader.loaded.connect(function() {
                        contextMenuLoader.open(item)
                    })
                    return
                }

                if (item) {
                    contextMenuLoader.row = item.index
                    contextMenuLoader.isLocal = item.isLocal
                }
                else {
                    contextMenuLoader.row = -1
                }

                const contextMenu = contextMenuLoader.item as Menu
                contextMenu.popup()
            }
        }

        Component {
            id: playlistItemWithThumbnail
            PlaylistItemWithThumbnail {
                m_mpv: root.m_mpv
            }
        }

        Component {
            id: playlistItemSimple
            PlaylistItem {
                m_mpv: root.m_mpv
            }
        }

        Component {
            id: playlistItemCompact
            PlaylistItemCompact {
                m_mpv: root.m_mpv
            }
        }

        // without a timer the scroll position is incorrect
        Timer {
            id: scrollPositionTimer

            interval: 100
            running: false
            repeat: false

            onTriggered: {
                playlistView.positionViewAtIndex(playlistView.model.playingVideo, ListView.Beginning)
            }
        }

        ShaderEffectSource {
            id: shaderEffect

            visible: PlaylistSettings.overlayVideo
            anchors.fill: playlistScrollView
            sourceItem: root.m_mpv
            sourceRect: {
                var rectTopLeftY = toolbar.visible ? toolbar.height : 0
                // future proof
                // let rectBotLeftY = footbar.visible ? footbar.height : 0
                if (PlaylistSettings.position === "right") {
                    return Qt.rect(
                        root.x,
                        root.m_mpv.y + rectTopLeftY,
                        root.width,
                        root.height - rectTopLeftY// - rectBotLeftY
                        )
                }
                else {
                    return Qt.rect(
                        root.x,
                        rectTopLeftY,
                        root.width,
                        root.height - rectTopLeftY// - rectBotLeftY
                        )
                }
            }
        }

        FastBlur {
            visible: PlaylistSettings.overlayVideo
            anchors.fill: shaderEffect
            radius: 100
            source: shaderEffect
            z: 10
        }
    }

    FileDialog {
        id: fileDialog

        property string fileType: "video"

        title: i18nc("@title:window", "Select file")
        currentFolder: GeneralSettings.fileDialogLastLocation
        fileMode: FileDialog.OpenFile
        nameFilters: {
            if (fileType === "playlist" ) {
                return ["m3u (*.m3u *.m3u8)"]
            } else {
                return []
            }
        }

        onAccepted: {
            switch (fileType) {
            case "video":
                root.m_mpv.visibleFilterProxyModel.addItems(fileDialog.selectedFiles, PlaylistModel.Append)
                break
            case "playlist":
                if (fileMode === FileDialog.OpenFile) {
                    root.m_mpv.visibleFilterProxyModel.addItem(fileDialog.selectedFile, PlaylistModel.Append)
                } else {
                    root.m_mpv.visibleFilterProxyModel.saveM3uFile(fileDialog.selectedFile)
                }

                break
            }
            GeneralSettings.fileDialogLastLocation = PathUtils.parentUrl(fileDialog.selectedFile)
            GeneralSettings.save()
        }
        onRejected: root.m_mpv.focus = true
        onVisibleChanged: {
            HarunaApp.actionsEnabled = !visible
        }
    }

    states: [
        State {
            name: "hidden"

            PropertyChanges {
                root.x: PlaylistSettings.position === "right"
                        ? root.Window.window.width
                        : -width
                root.visible: false
            }
        },
        State {
            name : "visible"

            PropertyChanges {
                root.x: PlaylistSettings.position === "right"
                        ? root.Window.window.width - root.width
                        : 0
                root.visible: true
            }
        }
    ]

    transitions: [
        Transition {
            from: "visible"
            to: "hidden"

            SequentialAnimation {
                NumberAnimation {
                    target: root
                    property: "x"
                    duration: Kirigami.Units.longDuration
                    easing.type: Easing.InQuad
                }

                PropertyAction {
                    target: root
                    property: "visible"
                    value: false
                }
            }
        },
        Transition {
            from: "hidden"
            to: "visible"

            SequentialAnimation {
                PropertyAction {
                    target: root
                    property: "visible"
                    value: true
                }

                NumberAnimation {
                    target: root
                    property: "x"
                    duration: Kirigami.Units.longDuration
                    easing.type: Easing.OutQuad
                }
            }
        }
    ]
}
