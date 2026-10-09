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
    property alias scrollPositionTimer: scrollPositionTimer
    property alias playlistView: playlistView

    property PlaylistsManager manager: PlaylistsManager {}

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

    PlaylistAdvancedSortWindow {
        id: advancedSortWindow

        playlistsManager: root.manager
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

                    playlistsManager: root.manager
                    Layout.alignment: Qt.AlignLeft | Qt.AlignBottom
                    Layout.fillWidth: true

                    Repeater {
                        model: root.manager.playlists
                        delegate: PlaylistTabDelegate {
                            playlistsManager: root.manager
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
                        delay: HarunaApp.isAltKeyPressed ? 0 : Kirigami.Units.toolTipDelay
                        visible: addPlaylistButton.hovered
                                 && (GeneralSettings.showExplanatoryToolTips || HarunaApp.isAltKeyPressed)
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
                                root.manager.visiblePlaylist.searchText = text
                                playlistView.positionViewAtIndex(0, ListView.Beginning)
                            }

                            Component.onCompleted: {
                                text = root.manager.visiblePlaylist.searchText
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
                            checked: root.manager.visiblePlaylist.sortOrder === Qt.AscendingOrder

                            icon {
                                name: "view-sort-ascending-name"
                                width: root.buttonSize
                                height: root.buttonSize
                            }

                            onTriggered: {
                                root.manager.visiblePlaylist.sortOrder = Qt.AscendingOrder
                            }

                            ActionGroup.group: sortOrderGroup
                        }
                        Kirigami.Action {
                            text: KI18n.i18nc("@action:button", "Descending")
                            checkable: true
                            checked: root.manager.visiblePlaylist.sortOrder === Qt.DescendingOrder

                            icon {
                                name: "view-sort-descending-name"
                                width: root.buttonSize
                                height: root.buttonSize
                            }
                            onTriggered: {
                                root.manager.visiblePlaylist.sortOrder = Qt.DescendingOrder
                            }

                            ActionGroup.group: sortOrderGroup
                        }

                        Kirigami.Action {
                            separator: true
                        }

                        Kirigami.Action {
                            checkable: true
                            checked: root.manager.visiblePlaylist.sortPreset === PlaylistSortProxyModel.None
                            text: KI18n.i18nc("@action:button", "None")

                            onTriggered: {
                                root.manager.visiblePlaylist.sortPreset = PlaylistSortProxyModel.None
                            }
                            ActionGroup.group: sortPresetGroup
                        }
                        Kirigami.Action {
                            checkable: true
                            checked: root.manager.visiblePlaylist.sortPreset === PlaylistSortProxyModel.FileName
                            text: KI18n.i18nc("@action:button", "File Name")

                            onTriggered: {
                                root.manager.visiblePlaylist.sortPreset = PlaylistSortProxyModel.FileName
                            }
                            ActionGroup.group: sortPresetGroup
                        }
                        Kirigami.Action {
                            checkable: true
                            checked: root.manager.visiblePlaylist.sortPreset === PlaylistSortProxyModel.Title
                            text: KI18n.i18nc("@action:button", "Title")

                            onTriggered: {
                                root.manager.visiblePlaylist.sortPreset = PlaylistSortProxyModel.Title
                            }
                            ActionGroup.group: sortPresetGroup
                        }
                        Kirigami.Action {
                            checkable: true
                            checked: root.manager.visiblePlaylist.sortPreset === PlaylistSortProxyModel.Duration
                            text: KI18n.i18nc("@action:button", "Duration")

                            onTriggered: {
                                root.manager.visiblePlaylist.sortPreset = PlaylistSortProxyModel.Duration
                            }
                            ActionGroup.group: sortPresetGroup
                        }
                        Kirigami.Action {
                            checkable: true
                            checked: root.manager.visiblePlaylist.sortPreset === PlaylistSortProxyModel.Date
                            text: KI18n.i18nc("@action:button", "Modified Date")

                            onTriggered: {
                                root.manager.visiblePlaylist.sortPreset = PlaylistSortProxyModel.Date
                            }
                            ActionGroup.group: sortPresetGroup
                        }
                        Kirigami.Action {
                            checkable: true
                            checked: root.manager.visiblePlaylist.sortPreset === PlaylistSortProxyModel.FileSize
                            text: KI18n.i18nc("@action:button", "File Size")

                            onTriggered: {
                                root.manager.visiblePlaylist.sortPreset = PlaylistSortProxyModel.FileSize
                            }
                            ActionGroup.group: sortPresetGroup
                        }
                        Kirigami.Action {
                            checkable: true
                            checked: root.manager.visiblePlaylist.sortPreset === PlaylistSortProxyModel.TrackNo
                            text: KI18n.i18nc("@action:button, as in 'Track no on a Audio CD', not 'subtitle track'", "Track No")

                            onTriggered: {
                                root.manager.visiblePlaylist.sortPreset = PlaylistSortProxyModel.TrackNo
                            }
                            ActionGroup.group: sortPresetGroup
                        }
                        Kirigami.Action {
                            checkable: true
                            checked: root.manager.visiblePlaylist.sortPreset === PlaylistSortProxyModel.SampleRate
                            text: KI18n.i18nc("@action:button", "Sample Rate")

                            onTriggered: {
                                root.manager.visiblePlaylist.sortPreset = PlaylistSortProxyModel.SampleRate
                            }
                            ActionGroup.group: sortPresetGroup
                        }
                        Kirigami.Action {
                            checkable: true
                            checked: root.manager.visiblePlaylist.sortPreset === PlaylistSortProxyModel.Bitrate
                            text: KI18n.i18nc("@action:button", "Bitrate")

                            onTriggered: {
                                root.manager.visiblePlaylist.sortPreset = PlaylistSortProxyModel.Bitrate
                            }
                            ActionGroup.group: sortPresetGroup
                        }

                        Kirigami.Action {
                            separator: true
                        }

                        Kirigami.Action {
                            checkable: true
                            checked: root.manager.visiblePlaylist.sortPreset === PlaylistSortProxyModel.Custom
                            text: KI18n.i18nc("@action:button", "Custom…")

                            onTriggered: {
                                root.advancedSortWindow.open()
                                root.manager.visiblePlaylist.itemsSorted()
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
                            root.manager.visiblePlaylist.clear()
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
                        displayHint: Kirigami.DisplayHint.AlwaysHide
                        enabled: !root.manager.visiblePlaylist.isUpdatingMetadata
                        onTriggered: {
                            root.manager.visiblePlaylist.updateMetadata()
                        }
                        tooltip: (GeneralSettings.showExplanatoryToolTips || HarunaApp.isAltKeyPressed)
                                 ? KI18n.i18nc("@info:tooltip", "Update metadata for all files in the playlist\n\n"+
                                               "Update metadata in the database with metadata inside the file.\n" +
                                               "Metadata is stored in the database for faster retrieval.")
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
            root.manager.visiblePlaylist.addItem(url, PlaylistModel.Append)
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
            root.manager.playlists.createNewPlaylist(plName)
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
                root.manager.visiblePlaylist.addFilesAndFolders(drop.urls, PlaylistModel.Append)
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

                delegate: {
                    switch (PlaylistSettings.style) {
                    case "default":
                        return playlistItemSimple
                    case "withThumbnails":
                        return playlistItemWithThumbnail
                    case "compact":
                        return playlistItemCompact
                    }
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
                    Qt.callLater(() => positionViewAtIndex(currentIndex, ListView.Beginning))
                }

                TapHandler {
                    acceptedButtons: Qt.MiddleButton
                    onSingleTapped: function(eventPoint, mouseButton) {
                        const index = root.manager.visiblePlaylist.getPlayingItem()
                        playlistView.positionViewAtIndex(index, ListView.Beginning)
                    }
                }
            }
        },

        Component {
            id: playlistItemWithThumbnail
            PlaylistItemWithThumbnail {
                m_mpv: root.m_mpv
                playlistsManager: root.manager
            }
        },

        Component {
            id: playlistItemSimple
            PlaylistItem {
                m_mpv: root.m_mpv
                playlistsManager: root.manager
            }
        },

        Component {
            id: playlistItemCompact
            PlaylistItemCompact {
                m_mpv: root.m_mpv
                playlistsManager: root.manager
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
                                root.x - root.m_mpv.x,
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
                root.manager.visiblePlaylist.addItems(fileDialog.selectedFiles, PlaylistModel.Append)
                break
            case "playlist":
                if (fileMode === FileDialog.OpenFile) {
                    root.manager.visiblePlaylist.addItem(fileDialog.selectedFile, PlaylistModel.Append)
                } else {
                    root.manager.visiblePlaylist.saveM3uFile(fileDialog.selectedFile)
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
