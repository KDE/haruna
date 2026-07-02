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

import org.kde.ki18n
import org.kde.kirigami as Kirigami

import org.kde.haruna
import org.kde.haruna.playlist
import org.kde.haruna.utilities
import org.kde.haruna.settings
import org.kde.haruna.youtube

ResizeablePage {
    id: root

    property alias advancedSortWindow: advancedSortWindow
    property alias manager: playlistsManager
    property alias scrollPositionTimer: scrollPositionTimer
    property alias playlistView: playlistView

    edge: PlaylistSettings.position === "right" ? Qt.RightEdge : Qt.LeftEdge
    customWidth: PlaylistSettings.playlistWidth
    width: limitWidth(customWidth * fsScale)

    function limitWidth(pWidth) {
        if (PlaylistSettings.style === "compact") {
            return 380
        } else {
            return Math.min(Math.max(pWidth, 260), mainWindowWidth - 50)
        }
    }

    onResize: function (delta) {
        // invert the drag delta when the playlist is anchored to the right
        // dragging left (pX is negative) expands a right-aligned playlist, but shrinks a left-aligned one
        const widthDelta = root.edge === Qt.RightEdge ? delta * -1 : delta;
        root.customWidth = root.limitWidth(root.customWidth + widthDelta)
    }

    onSaveWidth: {
        PlaylistSettings.playlistWidth = root.customWidth ? root.customWidth : 260
        PlaylistSettings.save()
    }

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

    PlaylistsManager {
        id: playlistsManager
    }

    PlaylistAdvancedSortWindow {
        id: advancedSortWindow

        playlistsManager: playlistsManager
    }

    header: ToolBar {
        id: toolbar

        visible: PlaylistSettings.showToolbar

        leftPadding: 1
        topPadding: 1
        rightPadding: 1
        bottomPadding: 0

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            RowLayout {
                PlaylistTabBar {
                    id: playlistTabView

                    playlistsManager: playlistsManager
                    Layout.alignment: Qt.AlignLeft | Qt.AlignBottom
                    Layout.fillWidth: true

                    Repeater {
                        model: playlistsManager.playlists
                        delegate: PlaylistTabDelegate {
                            playlistsManager: playlistsManager
                        }
                    }
                }

                ToolButton {
                    id: addPlaylistButton

                    icon.name: "list-add"
                    icon.width: root.buttonSize
                    icon.height: root.buttonSize
                    onClicked: {
                        if (addPlaylistPopup.opened) {
                            addPlaylistPopup.close()
                        } else {
                            addPlaylistPopup.open()
                        }
                    }

                    ToolTip {
                        text: KI18n.i18nc("@action:button", "Add new playlist")
                        visible: addPlaylistButton.hovered && GeneralSettings.showExplanatoryToolTips
                    }
                }
            }

            Kirigami.ActionToolBar {
                ActionGroup { id: sortOrderGroup }
                ActionGroup { id: sortPresetGroup }
                actions: [
                    Kirigami.Action {
                        text: KI18n.i18nc("@action:button", "Search")
                        icon.name: "search"
                        icon.width: root.buttonSize
                        icon.height: root.buttonSize
                        displayHint: Kirigami.DisplayHint.IconOnly
                        displayComponent: Kirigami.SearchField {
                            id: searchComponent
                            delaySearch: true

                            onTextChanged: {
                                playlistsManager.visiblePlaylist.searchText = text
                                playlistView.positionViewAtIndex(0, ListView.Beginning)
                            }

                            Component.onCompleted: {
                                text = playlistsManager.visiblePlaylist.searchText
                            }
                        }
                    },
                    Kirigami.Action {
                        text: KI18n.i18nc("@action:button", "Add…")
                        displayHint: root.isSmallWindowSize
                                     ? Kirigami.DisplayHint.IconOnly
                                     : Kirigami.DisplayHint.NoPreference
                        icon.name: "list-add"
                        icon.width: root.buttonSize
                        icon.height: root.buttonSize
                        Kirigami.Action {
                            text: KI18n.i18nc("@action:button", "Files")
                            onTriggered: {
                                fileDialog.fileType = "video"
                                fileDialog.fileMode = FileDialog.OpenFiles
                                fileDialog.open()
                            }
                        }
                        Kirigami.Action {
                            text: KI18n.i18nc("@action:button", "URL")
                            onTriggered: {
                                if (addUrlPopup.opened) {
                                    addUrlPopup.close()
                                } else {
                                    addUrlPopup.open()
                                }
                            }
                        }
                        Kirigami.Action {
                            text: KI18n.i18nc("@action:button", "Playlist")
                            onTriggered: {
                                fileDialog.fileType = "playlist"
                                fileDialog.fileMode = FileDialog.OpenFile
                                fileDialog.open()
                            }
                        }
                    },
                    Kirigami.Action {
                        text: KI18n.i18nc("@action:button", "Sort")
                        displayHint: root.isSmallWindowSize
                                     ? Kirigami.DisplayHint.IconOnly
                                     : Kirigami.DisplayHint.NoPreference
                        icon.name: "view-sort"
                        icon.width: root.buttonSize
                        icon.height: root.buttonSize

                        Kirigami.Action {
                            text: KI18n.i18nc("@action:button", "Ascending")
                            checkable: true
                            checked: playlistsManager.visiblePlaylist.sortOrder === Qt.AscendingOrder

                            icon {
                                name: "view-sort-ascending-name"
                                width: root.buttonSize
                                height: root.buttonSize
                            }

                            onTriggered: {
                                playlistsManager.visiblePlaylist.sortOrder = Qt.AscendingOrder
                            }

                            ActionGroup.group: sortOrderGroup
                        }
                        Kirigami.Action {
                            text: KI18n.i18nc("@action:button", "Descending")
                            checkable: true
                            checked: playlistsManager.visiblePlaylist.sortOrder === Qt.DescendingOrder

                            icon {
                                name: "view-sort-descending-name"
                                width: root.buttonSize
                                height: root.buttonSize
                            }
                            onTriggered: {
                                playlistsManager.visiblePlaylist.sortOrder = Qt.DescendingOrder
                            }

                            ActionGroup.group: sortOrderGroup
                        }

                        Kirigami.Action {
                            separator: true
                        }

                        Kirigami.Action {
                            checkable: true
                            checked: playlistsManager.visiblePlaylist.sortPreset === PlaylistSortProxyModel.None
                            text: KI18n.i18nc("@action:button", "None")

                            onTriggered: {
                                playlistsManager.visiblePlaylist.sortPreset = PlaylistSortProxyModel.None
                            }
                            ActionGroup.group: sortPresetGroup
                        }
                        Kirigami.Action {
                            checkable: true
                            checked: playlistsManager.visiblePlaylist.sortPreset === PlaylistSortProxyModel.FileName
                            text: KI18n.i18nc("@action:button", "File Name")

                            onTriggered: {
                                playlistsManager.visiblePlaylist.sortPreset = PlaylistSortProxyModel.FileName
                            }
                            ActionGroup.group: sortPresetGroup
                        }
                        Kirigami.Action {
                            checkable: true
                            checked: playlistsManager.visiblePlaylist.sortPreset === PlaylistSortProxyModel.Title
                            text: KI18n.i18nc("@action:button", "Title")

                            onTriggered: {
                                playlistsManager.visiblePlaylist.sortPreset = PlaylistSortProxyModel.Title
                            }
                            ActionGroup.group: sortPresetGroup
                        }
                        Kirigami.Action {
                            checkable: true
                            checked: playlistsManager.visiblePlaylist.sortPreset === PlaylistSortProxyModel.Duration
                            text: KI18n.i18nc("@action:button", "Duration")

                            onTriggered: {
                                playlistsManager.visiblePlaylist.sortPreset = PlaylistSortProxyModel.Duration
                            }
                            ActionGroup.group: sortPresetGroup
                        }
                        Kirigami.Action {
                            checkable: true
                            checked: playlistsManager.visiblePlaylist.sortPreset === PlaylistSortProxyModel.Date
                            text: KI18n.i18nc("@action:button", "Modified Date")

                            onTriggered: {
                                playlistsManager.visiblePlaylist.sortPreset = PlaylistSortProxyModel.Date
                            }
                            ActionGroup.group: sortPresetGroup
                        }
                        Kirigami.Action {
                            checkable: true
                            checked: playlistsManager.visiblePlaylist.sortPreset === PlaylistSortProxyModel.FileSize
                            text: KI18n.i18nc("@action:button", "File Size")

                            onTriggered: {
                                playlistsManager.visiblePlaylist.sortPreset = PlaylistSortProxyModel.FileSize
                            }
                            ActionGroup.group: sortPresetGroup
                        }
                        Kirigami.Action {
                            checkable: true
                            checked: playlistsManager.visiblePlaylist.sortPreset === PlaylistSortProxyModel.TrackNo
                            text: KI18n.i18nc("@action:button, as in 'Track no on a Audio CD', not 'subtitle track'", "Track No")

                            onTriggered: {
                                playlistsManager.visiblePlaylist.sortPreset = PlaylistSortProxyModel.TrackNo
                            }
                            ActionGroup.group: sortPresetGroup
                        }
                        Kirigami.Action {
                            checkable: true
                            checked: playlistsManager.visiblePlaylist.sortPreset === PlaylistSortProxyModel.SampleRate
                            text: KI18n.i18nc("@action:button", "Sample Rate")

                            onTriggered: {
                                playlistsManager.visiblePlaylist.sortPreset = PlaylistSortProxyModel.SampleRate
                            }
                            ActionGroup.group: sortPresetGroup
                        }
                        Kirigami.Action {
                            checkable: true
                            checked: playlistsManager.visiblePlaylist.sortPreset === PlaylistSortProxyModel.Bitrate
                            text: KI18n.i18nc("@action:button", "Bitrate")

                            onTriggered: {
                                playlistsManager.visiblePlaylist.sortPreset = PlaylistSortProxyModel.Bitrate
                            }
                            ActionGroup.group: sortPresetGroup
                        }

                        Kirigami.Action {
                            separator: true
                        }

                        Kirigami.Action {
                            checkable: true
                            checked: playlistsManager.visiblePlaylist.sortPreset === PlaylistSortProxyModel.Custom
                            text: KI18n.i18nc("@action:button", "Custom…")

                            onTriggered: {
                                root.advancedSortWindow.open()
                                playlistsManager.visiblePlaylist.itemsSorted()
                            }
                            ActionGroup.group: sortPresetGroup
                        }
                    },

                    Kirigami.Action {
                        text: KI18n.i18nc("@action:button", "Playback")
                        icon.name: "media-playback-start"
                        icon.width: root.buttonSize
                        icon.height: root.buttonSize
                        displayHint: Kirigami.DisplayHint.KeepVisible
                        Kirigami.Action {
                            text: KI18n.i18nc("@action:button", "Repeat playlist")
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
                            text: KI18n.i18nc("@action:button", "Stop after last item")
                            autoExclusive: true
                            checkable: true
                            checked: PlaylistSettings.playbackBehavior === "StopAfterLast"
                            onTriggered: {
                                PlaylistSettings.playbackBehavior = "StopAfterLast"
                                PlaylistSettings.save()
                            }
                        }
                        Kirigami.Action {
                            text: KI18n.i18nc("@action:button", "Repeat item")
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
                            text: KI18n.i18nc("@action:button", "Stop after item")
                            autoExclusive: true
                            checkable: true
                            checked: PlaylistSettings.playbackBehavior === "StopAfterItem"
                            onTriggered: {
                                PlaylistSettings.playbackBehavior = "StopAfterItem"
                                PlaylistSettings.save()
                            }
                        }
                        Kirigami.Action {
                            text: KI18n.i18nc("@action:button", "Random Playback")
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
                        text: KI18n.i18nc("@action:button", "Clear")
                        icon.name: "edit-clear-all"
                        displayHint: Kirigami.DisplayHint.AlwaysHide
                        onTriggered: {
                            playlistsManager.visiblePlaylist.clear()
                        }
                    },
                    Kirigami.Action {
                        text: KI18n.i18nc("@action:button", "Save As")
                        icon.name: "document-save-as"
                        displayHint: Kirigami.DisplayHint.AlwaysHide
                        onTriggered: {
                            fileDialog.fileType = "playlist"
                            fileDialog.fileMode = FileDialog.SaveFile
                            fileDialog.open()
                        }
                    },
                    Kirigami.Action {
                        text: KI18n.i18nc("@action:inmenu", "Update all metadata")
                        icon.name: "view-refresh"
                        enabled: !playlistsManager.visiblePlaylist.isUpdatingMetadata
                        onTriggered: {
                            playlistsManager.visiblePlaylist.updateMetadata()
                        }
                        tooltip: GeneralSettings.showExplanatoryToolTips
                                 ? KI18n.i18nc("@info:tooltip", "Update metadata for all files in the playlist\n\n"+
                                               "Update metadata in the database with metadata inside the file.\n" +
                                               "Metadata is stored in the database for faster retrival.")
                                 : ""
                    }
                ]
            }
        }
    }

    InputPopup {
        id: addUrlPopup

        x: Kirigami.Units.largeSpacing
        y: Kirigami.Units.largeSpacing
        width: toolbar.width - Kirigami.Units.largeSpacing * 2
        buttonText: KI18n.i18nc("@action:button", "Add")
        warningText: youtube.hasYoutubeDl()
                     ? ""
                     : KI18n.i18nc("@info", "Neither <a href=\"https://github.com/yt-dlp/yt-dlp\">yt-dlp</a> nor <a href=\"https://github.com/ytdl-org/youtube-dl\">youtube-dl</a> was found.")

        onSubmitted: function(url) {
            playlistsManager.visiblePlaylist.addItem(url, PlaylistModel.Append)
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
        placeholderText: KI18n.i18nc("@placeholder", "playlist name")
        buttonText: KI18n.i18nc("@action:button", "Add")

        onSubmitted: function(plName) {
            playlistsManager.playlists.createNewPlaylist(plName)
        }
    }

    pageContent: [       
        DropArea {
            id: playlistDropArea

            anchors.fill: playlistScrollView
            keys: ["text/uri-list"]

            onDropped: function(drop) {
                if (!containsDrag) {
                    return
                }
                playlistsManager.visiblePlaylist.addFilesAndFolders(drop.urls, PlaylistModel.Append)
            }
        },

        ScrollView {
            id: playlistScrollView

            z: 20
            anchors.fill: parent
            anchors {
                leftMargin: root.pageEdgeBorder.width
                rightMargin: root.pageEdgeBorder.width
            }

            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

            ListView {
                id: playlistView

                property bool startupScrollDone: false

                // set bottomMargin so that the footer doesn't block playlist items
                bottomMargin: GeneralSettings.footerStyle === "default" ? 0 : 100

                model: playlistsManager.visiblePlaylist
                onModelChanged: {
                    Qt.callLater(playlistsManager.visiblePlaylist.refreshData)
                }

                reuseItems: true
                spacing: 1
                currentIndex: playlistsManager.visiblePlaylist.getPlayingItem()
                highlightFollowsCurrentItem: false

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
                        model: playlistsManager.visiblePlaylist
                    }
                }

                onCountChanged: {
                    if (startupScrollDone || count <= 0) {
                        return
                    }
                    startupScrollDone = true
                    Qt.callLater(() => positionViewAtIndex(currentIndex, ListView.Beginning))
                }

                MouseArea {
                    z: -1
                    anchors.fill: playlistView
                    acceptedButtons: Qt.MiddleButton | Qt.RightButton
                    onClicked: function(mouse) {
                        switch (mouse.button) {
                        case Qt.MiddleButton:
                            const index = playlistsManager.visiblePlaylist.getPlayingItem()
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
        },

        Loader {
            id: contextMenuLoader

            property int row: -1
            property bool isLocal: false

            active: false
            asynchronous: true
            sourceComponent: Menu {

                MenuItem {
                    text: KI18n.i18nc("@action:inmenu", "Open Containing Folder")
                    icon.name: "folder"
                    visible: contextMenuLoader.isLocal && contextMenuLoader.row != -1
                    onClicked: playlistsManager.visiblePlaylist.highlightInFileManager(contextMenuLoader.row)
                }
                MenuItem {
                    text: KI18n.i18nc("@action:inmenu", "Open in Browser")
                    icon.name: "link"
                    visible: !contextMenuLoader.isLocal && contextMenuLoader.row != -1
                    onClicked: {
                        const modelIndex = playlistsManager.visiblePlaylist.index(contextMenuLoader.row, 0)
                        Qt.openUrlExternally(modelIndex.data(PlaylistModel.PathRole))
                    }
                }
                MenuItem {
                    text: KI18n.i18nc("@action:inmenu", "Open in Thumbnail Generator")
                    icon.name: "hana"
                    visible: {
                        const modelIndex = playlistsManager.visiblePlaylist.index(contextMenuLoader.row, 0)
                        return modelIndex.data(PlaylistModel.TypeRole) === "video"
                             && SystemUtils.isHanaInstalled()
                             && SystemUtils.platformName() !== "windows"
                             && contextMenuLoader.isLocal && contextMenuLoader.row != -1
                    }
                    onClicked: {
                        const modelIndex = playlistsManager.visiblePlaylist.index(contextMenuLoader.row, 0)
                        const url = modelIndex.data(PlaylistModel.PathRole)
                        SystemUtils.openHana(url)
                    }
                }
                MenuItem {
                    text: KI18n.i18nc("@action:inmenu %1 is 'MediaInfo' (app name)", "Open in %1", "MediaInfo")
                    visible: SystemUtils.isMediaInfoInstalled()
                             && SystemUtils.platformName() !== "windows"
                             && contextMenuLoader.isLocal && contextMenuLoader.row != -1
                    onClicked: {
                        const modelIndex = playlistsManager.visiblePlaylist.index(contextMenuLoader.row, 0)
                        const url = modelIndex.data(PlaylistModel.PathRole)
                        SystemUtils.openMediaInfo(url)
                    }
                }
                MenuItem {
                    text: KI18n.i18nc("@action:inmenu", "Copy Name")
                    onClicked: playlistsManager.visiblePlaylist.copyFileName(contextMenuLoader.row)
                    visible: contextMenuLoader.row != -1
                }
                MenuItem {
                    text: contextMenuLoader.isLocal
                          ? KI18n.i18nc("@action:inmenu", "Copy Path")
                          : KI18n.i18nc("@action:inmenu", "Copy URL")
                    onClicked: playlistsManager.visiblePlaylist.copyFilePath(contextMenuLoader.row)
                    visible: contextMenuLoader.row != -1
                }

                MenuItem {
                    id: updateMetadataMenuItem

                    text: KI18n.i18nc("@action:inmenu", "Update metadata")
                    icon.name: "view-refresh"
                    visible: contextMenuLoader.row != -1
                    onClicked: {
                        const modelIndex = playlistsManager.visiblePlaylist.index(contextMenuLoader.row, 0)
                        const url = modelIndex.data(PlaylistModel.PathRole)
                        Database.updateMetadata(url)
                    }

                    ToolTip {
                        text: KI18n.i18nc("@info:tooltip", "Update metadata in the database with metadata inside the file.\n" +
                                          "Metadata is stored in the database for faster retrival.")
                        visible: updateMetadataMenuItem.hovered && GeneralSettings.showExplanatoryToolTips
                    }
                }

                MenuSeparator {
                    visible: contextMenuLoader.row != -1
                }

                // Selection manipulators
                MenuItem {
                    text: KI18n.i18nc("@action:inmenu", "Select All")
                    onClicked: playlistsManager.visiblePlaylist.selectItem(0, PlaylistFilterProxyModel.All)
                }
                MenuItem {
                    text: KI18n.i18nc("@action:inmenu", "Deselect All")
                    onClicked: playlistsManager.visiblePlaylist.selectItem(0, PlaylistFilterProxyModel.Clear)
                }
                MenuItem {
                    text: KI18n.i18nc("@action:inmenu", "Invert Selection")
                    onClicked: playlistsManager.visiblePlaylist.selectItem(0, PlaylistFilterProxyModel.Invert)
                }
                MenuSeparator {}
                MenuItem {
                    text: KI18n.i18nc("@action:inmenu", "Remove from Playlist")
                    icon.name: "remove"
                    onClicked: playlistsManager.visiblePlaylist.removeItem(contextMenuLoader.row)
                    visible: playlistsManager.visiblePlaylist.selectionCount === 1 && contextMenuLoader.row != -1
                }
                MenuItem {
                    text: KI18n.i18nc("@action:inmenu", "Remove Selected from Playlist")
                    icon.name: "remove"
                    onClicked: playlistsManager.visiblePlaylist.removeItems()
                    visible: playlistsManager.visiblePlaylist.selectionCount > 1 && contextMenuLoader.row != -1
                }
                MenuItem {
                    text: KI18n.i18nc("@action:inmenu", "Rename")
                    icon.name: "edit-rename"
                    visible: contextMenuLoader.isLocal && contextMenuLoader.row != -1
                    onClicked: playlistsManager.visiblePlaylist.renameFile(contextMenuLoader.row)
                }
                MenuItem {
                    text: KI18n.i18nc("@action:inmenu", "Scroll to Playing Item")
                    onClicked: {
                        const index = playlistsManager.visiblePlaylist.getPlayingItem()
                        playlistView.positionViewAtIndex(index, ListView.Beginning)
                    }
                }
                MenuSeparator {
                    visible: contextMenuLoader.isLocal && contextMenuLoader.row != -1
                }
                MenuItem {
                    text: KI18n.i18nc("@action:inmenu", "Move File to Trash")
                    icon.name: "delete"
                    visible: contextMenuLoader.isLocal && playlistsManager.visiblePlaylist.selectionCount === 1 && contextMenuLoader.row != -1
                    onClicked: playlistsManager.visiblePlaylist.trashFile(contextMenuLoader.row)
                }
                MenuItem {
                    text: KI18n.i18nc("@action:inmenu", "Move Selected Files to Trash")
                    icon.name: "delete"
                    visible: contextMenuLoader.isLocal && playlistsManager.visiblePlaylist.selectionCount > 1 && contextMenuLoader.row != -1
                    onClicked: playlistsManager.visiblePlaylist.trashFiles()
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
        },

        Component {
            id: playlistItemWithThumbnail
            PlaylistItemWithThumbnail {
                m_mpv: root.m_mpv
                playlistsManager: playlistsManager
            }
        },

        Component {
            id: playlistItemSimple
            PlaylistItem {
                m_mpv: root.m_mpv
                playlistsManager: playlistsManager
            }
        },

        Component {
            id: playlistItemCompact
            PlaylistItemCompact {
                m_mpv: root.m_mpv
                playlistsManager: playlistsManager
            }
        },

        // without a timer the scroll position is incorrect
        Timer {
            id: scrollPositionTimer

            interval: 100
            running: false
            repeat: false

            onTriggered: {
                playlistView.positionViewAtIndex(playlistView.model.playingVideo, ListView.Beginning)
            }
        },

        ShaderEffectSource {
            id: shaderEffect

            visible: PlaylistSettings.overlayVideo
            anchors.fill: playlistScrollView
            sourceItem: root.m_mpv
            sourceRect: {
                var rectTopLeftY = toolbar.visible ? toolbar.height : 0
                if (PlaylistSettings.position === "right") {
                    return Qt.rect(
                                root.x,
                                root.m_mpv.y + rectTopLeftY,
                                root.width,
                                root.height - rectTopLeftY
                                )
                } else {
                    return Qt.rect(
                                root.x,
                                rectTopLeftY,
                                root.width,
                                root.height - rectTopLeftY
                                )
                }
            }
        },

        FastBlur {
            visible: PlaylistSettings.overlayVideo
            anchors.fill: shaderEffect
            radius: 100
            source: shaderEffect
        }
    ]

    FileDialog {
        id: fileDialog

        property string fileType: "video"

        title: KI18n.i18nc("@title:window", "Select file")
        currentFolder: GeneralSettings.fileDialogLastLocation
        fileMode: FileDialog.OpenFile
        nameFilters: {
            if (fileType === "playlist" ) {
                return ["m3u (*.m3u *.m3u8)"]
            } else {
                return [""]
            }
        }

        onAccepted: {
            switch (fileType) {
            case "video":
                playlistsManager.visiblePlaylist.addItems(fileDialog.selectedFiles, PlaylistModel.Append)
                break
            case "playlist":
                if (fileMode === FileDialog.OpenFile) {
                    playlistsManager.visiblePlaylist.addItem(fileDialog.selectedFile, PlaylistModel.Append)
                } else {
                    playlistsManager.visiblePlaylist.saveM3uFile(fileDialog.selectedFile)
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
}
