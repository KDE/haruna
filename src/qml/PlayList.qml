/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
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
import org.kde.haruna.settings

Page {
    id: root

    required property MpvVideo m_mpv

    property bool isSmallWindowSize: Window.window.width < 600
    property int buttonSize: isSmallWindowSize ? Kirigami.Units.iconSizes.small : Kirigami.Units.iconSizes.smallMedium
    property alias scrollPositionTimer: scrollPositionTimer
    property alias playlistView: playlistView

    height: m_mpv.height
    width: {
        if (PlaylistSettings.style === "compact") {
            return 380
        } else {
            let playlistWidth = Window.window.width - 50
            if (Window.window.width > 600) {
                playlistWidth = 550
            }
            if (Window.window.width * 0.35 > playlistWidth) {
                playlistWidth = Window.window.width * 0.35
            }
            return playlistWidth
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
                actions: [
                    Kirigami.Action {
                        text: i18nc("@action:button", "Open Playlist")
                        displayHint: root.isSmallWindowSize
                                     ? Kirigami.DisplayHint.IconOnly
                                     : Kirigami.DisplayHint.NoPreference
                        icon.name: "media-playlist-append"
                        icon.width: root.buttonSize
                        icon.height: root.buttonSize
                        onTriggered: {
                            fileDialog.fileType = "playlist"
                            fileDialog.fileMode = FileDialog.OpenFile
                            fileDialog.open()
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
                                if (addPlaylistPopup.opened) {
                                    addPlaylistPopup.close()
                                } else {
                                    addPlaylistPopup.open()
                                }
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
                            text: i18nc("@action:button", "Name, ascending")
                            onTriggered: {
                                root.m_mpv.visibleFilterProxyModel.sortItems(PlaylistSortProxyModel.NameAscending)
                            }
                        }
                        Kirigami.Action {
                            text: i18nc("@action:button", "Name, descending")
                            onTriggered: {
                                root.m_mpv.visibleFilterProxyModel.sortItems(PlaylistSortProxyModel.NameDescending)
                            }
                        }
                        Kirigami.Action {
                            text: i18nc("@action:button", "Duration, ascending")
                            onTriggered: {
                                root.m_mpv.visibleFilterProxyModel.sortItems(PlaylistSortProxyModel.DurationAscending)
                            }
                        }
                        Kirigami.Action {
                            text: i18nc("@action:button", "Duration, descending")
                            onTriggered: {
                                root.m_mpv.visibleFilterProxyModel.sortItems(PlaylistSortProxyModel.DurationDescending)
                            }
                        }
                    },
                    Kirigami.Action {
                        text: i18nc("@action:button", "Playback")
                        icon.name: ""
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
                            text: i18nc("@action:button", "Random")
                            autoExclusive: true
                            checkable: true
                            checked: PlaylistSettings.playbackBehavior === "Random"
                            icon.name: "randomize"
                            onTriggered: {
                                PlaylistSettings.playbackBehavior = "Random"
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

        onUrlOpened: function(url) {
            root.m_mpv.visibleFilterProxyModel.addItem(url, PlaylistModel.Append)
        }
    }

    InputPopup {
        id: addPlaylistPopup

        x: Kirigami.Units.largeSpacing
        y: Kirigami.Units.largeSpacing
        width: toolbar.width - Kirigami.Units.largeSpacing * 2
        buttonText: i18nc("@action:button", "Add")

        onUrlOpened: function(url) {
            root.m_mpv.playlists.addPlaylist(url)
        }
    }

    Rectangle {

        Rectangle {
            z: 20
            width: 1
            height: parent.height
            color: Kirigami.Theme.backgroundColor
        }

        anchors.fill: parent
        color: Kirigami.Theme.backgroundColor

        ScrollView {
            id: playlistScrollView

            z: 20
            anchors.fill: parent
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
                    onClicked: root.m_mpv.visibleFilterProxyModel.selectItem(0, visibleFilterProxyModel.All)
                }
                MenuItem {
                    text: i18nc("@action:inmenu", "Deselect All")
                    onClicked: root.m_mpv.visibleFilterProxyModel.selectItem(0, visibleFilterProxyModel.Clear)
                }
                MenuItem {
                    text: i18nc("@action:inmenu", "Invert Selection")
                    onClicked: root.m_mpv.visibleFilterProxyModel.selectItem(0, visibleFilterProxyModel.Invert)
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
            PlayListItemWithThumbnail {
                m_mpv: root.m_mpv
            }
        }

        Component {
            id: playlistItemSimple
            PlayListItem {
                m_mpv: root.m_mpv
            }
        }

        Component {
            id: playlistItemCompact
            PlayListItemCompact {
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
        currentFolder: HarunaApp.pathToUrl(GeneralSettings.fileDialogLastLocation)
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
        }
        onRejected: root.m_mpv.focus = true
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
