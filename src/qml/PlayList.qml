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

Item {
    id: root

    required property MpvVideo m_mpv

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

    Rectangle {

        Rectangle {
            z: 20
            width: 1
            height: parent.height
            color: {
                let color = Kirigami.Theme.backgroundColor
                Qt.hsla(color.hslHue, color.hslSaturation, color.hslLightness, 0.12)
            }
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
                bottomMargin: GeneralSettings.footerStyle === "default" ? 0 : 65
                model: root.m_mpv.playlistProxyModel
                reuseItems: true
                spacing: 1
                currentIndex: root.m_mpv.playlistProxyModel.getPlayingItem()

                headerPositioning: ListView.OverlayHeader
                header: ToolBar {
                    id: toolbar

                    z: 100
                    width: parent.width
                    visible: PlaylistSettings.showToolbar
                    height: visible ? implicitHeight : 0

                    InputPopup {
                        id: addUrlPopup

                        width: toolbar.width - Kirigami.Units.largeSpacing
                        buttonText: i18nc("@action:button", "Add")

                        onUrlOpened: function(url) {
                            root.m_mpv.playlistModel.addItem(url, PlaylistModel.Append)
                        }
                    }

                    RowLayout {
                        anchors.fill: parent
                        Kirigami.ActionToolBar {
                            actions: [
                                Kirigami.Action {
                                    text: i18nc("@action:button", "Open playlist")
                                    icon.name: "media-playlist-append"
                                    onTriggered: {
                                        fileDialog.fileType = "playlist"
                                        fileDialog.fileMode = FileDialog.OpenFile
                                        fileDialog.open()
                                    }
                                },
                                Kirigami.Action {
                                    text: i18nc("@action:button", "Add ...")
                                    icon.name: "list-add"
                                    Kirigami.Action {
                                        text: i18nc("@action:button", "File")
                                        onTriggered: {
                                            fileDialog.fileType = "video"
                                            fileDialog.fileMode = FileDialog.OpenFile
                                            fileDialog.open()
                                        }
                                    }
                                    Kirigami.Action {
                                        text: i18nc("@action:button", "Url")
                                        onTriggered: {
                                            if (addUrlPopup.opened) {
                                                addUrlPopup.close()
                                            } else {
                                                addUrlPopup.open()
                                            }
                                        }
                                    }
                                },
                                Kirigami.Action {
                                    text: i18nc("@action:button", "Sort")
                                    icon.name: "view-sort"

                                    Kirigami.Action {
                                        text: i18nc("@action:button", "Name ascending")
                                        onTriggered: {
                                            root.m_mpv.playlistProxyModel.sortItems(PlaylistProxyModel.NameAscending)
                                        }
                                    }
                                    Kirigami.Action {
                                        text: i18nc("@action:button", "Name descending")
                                        onTriggered: {
                                            root.m_mpv.playlistProxyModel.sortItems(PlaylistProxyModel.NameDescending)
                                        }
                                    }
                                    Kirigami.Action {
                                        text: i18nc("@action:button", "Duration ascending")
                                        onTriggered: {
                                            root.m_mpv.playlistProxyModel.sortItems(PlaylistProxyModel.DurationAscending)
                                        }
                                    }
                                    Kirigami.Action {
                                        text: i18nc("@action:button", "Duration descending")
                                        onTriggered: {
                                            root.m_mpv.playlistProxyModel.sortItems(PlaylistProxyModel.DurationDescending)
                                        }
                                    }
                                },
                                Kirigami.Action {
                                    text: i18nc("@action:button", "Clear")
                                    icon.name: "edit-clear-all"
                                    displayHint: Kirigami.DisplayHint.AlwaysHide
                                    onTriggered: {
                                        root.m_mpv.playlistModel.clear()
                                    }
                                },
                                Kirigami.Action {
                                    text: i18nc("@action:button", "Save as")
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

                MouseArea {
                    anchors.fill: playlistView.contentItem
                    acceptedButtons: Qt.MiddleButton | Qt.RightButton
                    onClicked: function(mouse) {
                        switch (mouse.button) {
                        case Qt.MiddleButton:
                            const index = root.m_mpv.playlistProxyModel.getPlayingItem()
                            playlistView.positionViewAtIndex(index, ListView.Beginning)
                            break
                        case Qt.RightButton:
                            const item = playlistView.itemAt(mouseX, mouseY)
                            if (!item) {
                                return
                            }

                            contextMenuLoader.open(item)
                            break
                        }
                    }
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
                    text: i18nc("@action:inmenu", "Open containing folder")
                    icon.name: "folder"
                    visible: contextMenuLoader.isLocal
                    onClicked: root.m_mpv.playlistProxyModel.highlightInFileManager(contextMenuLoader.row)
                }
                MenuItem {
                    text: i18nc("@action:inmenu", "Open in browser")
                    icon.name: "link"
                    visible: !contextMenuLoader.isLocal
                    onClicked: {
                        const modelIndex = root.m_mpv.playlistProxyModel.index(contextMenuLoader.row, 0)
                        Qt.openUrlExternally(modelIndex.data(PlaylistModel.PathRole))
                    }
                }
                MenuItem {
                    text: i18nc("@action:inmenu", "Copy name")
                    onClicked: root.m_mpv.playlistProxyModel.copyFileName(contextMenuLoader.row)
                }
                MenuItem {
                    text: contextMenuLoader.isLocal
                          ? i18nc("@action:inmenu", "Copy path")
                          : i18nc("@action:inmenu", "Copy url")
                    onClicked: root.m_mpv.playlistProxyModel.copyFilePath(contextMenuLoader.row)
                }
                MenuSeparator {}
                MenuItem {
                    text: i18nc("@action:inmenu", "Remove from playlist")
                    icon.name: "remove"
                    onClicked: root.m_mpv.playlistProxyModel.removeItem(contextMenuLoader.row)
                }
                MenuItem {
                    text: i18nc("@action:inmenu", "Rename")
                    icon.name: "edit-rename"
                    visible: contextMenuLoader.isLocal
                    onClicked: root.m_mpv.playlistProxyModel.renameFile(contextMenuLoader.row)
                }
                MenuItem {
                    text: i18nc("@action:inmenu", "Scroll to playing item")
                    onClicked: {
                        const index = root.m_mpv.playlistProxyModel.getPlayingItem()
                        playlistView.positionViewAtIndex(index, ListView.Beginning)
                    }
                }
                MenuSeparator {
                    visible: contextMenuLoader.isLocal
                }
                MenuItem {
                    text: i18nc("@action:inmenu", "Trash file")
                    icon.name: "delete"
                    visible: contextMenuLoader.isLocal
                    onClicked: root.m_mpv.playlistProxyModel.trashFile(contextMenuLoader.row)
                }
            }

            function open(item: ItemDelegate) : void {
                if (!contextMenuLoader.active) {
                    contextMenuLoader.active = true
                    contextMenuLoader.loaded.connect(function() {
                        contextMenuLoader.open(item)
                    })
                    return
                }
                let playlistItem = null
                switch (PlaylistSettings.style) {
                case "default": {
                    playlistItem = item as PlayListItem
                    break
                }
                case "withThumbnails": {
                    playlistItem = item as PlayListItemWithThumbnail
                    break
                }
                case "compact": {
                    playlistItem = item as PlayListItemCompact
                    break
                }
                }
                if (playlistItem === null) {
                    return
                }

                contextMenuLoader.row = item.index
                contextMenuLoader.isLocal = item.isLocal

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
            sourceRect: PlaylistSettings.position === "right"
                        ? Qt.rect(root.m_mpv.width - root.width, root.m_mpv.y, root.width, root.height)
                        : Qt.rect(0, 0, root.width, root.height)
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

        property url location: GeneralSettings.fileDialogLocation
                               ? HarunaApp.pathToUrl(GeneralSettings.fileDialogLocation)
                               : HarunaApp.pathToUrl(GeneralSettings.fileDialogLastLocation)
        property string fileType: "video"

        title: i18nc("@title:window", "Select file")
        currentFolder: location
        fileMode: FileDialog.OpenFile

        onAccepted: {
            switch (fileType) {
            case "video":
                root.m_mpv.playlistModel.addItem(fileDialog.selectedFile, PlaylistModel.Append)
                break
            case "playlist":
                if (fileMode === FileDialog.OpenFile) {
                    root.m_mpv.playlistModel.addItem(fileDialog.selectedFile, PlaylistModel.Append)
                } else {
                    root.m_mpv.playlistProxyModel.saveM3uFile(fileDialog.selectedFile)
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
                root.x: PlaylistSettings.position === "right" ? root.Window.window.width : -width
                root.visible: false
            }
        },
        State {
            name : "visible"

            PropertyChanges {
                root.x: PlaylistSettings.position === "right" ? root.Window.window.width - root.width : 0
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
                    duration: 150
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
                    duration: 150
                    easing.type: Easing.OutQuad
                }
            }
        }
    ]
}
