/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import Qt.labs.platform as Platform

import org.kde.kirigami as Kirigami
import org.kde.haruna
import org.kde.haruna.models
import Haruna.Components

Item {
    id: root

    property alias scrollPositionTimer: scrollPositionTimer
    property alias playlistView: playlistView

    height: mpv.height
    width: {
        if (PlaylistSettings.style === "compact") {
            return Kirigami.Units.gridUnit * 21
        } else {
            const w = Kirigami.Units.gridUnit * 31
            return (window.width * 0.33) < w ? w : window.width * 0.33
        }
    }
    x: PlaylistSettings.position === "right" ? window.width : -width
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

                model: mpv.playlistProxyModel
                spacing: 1
                currentIndex: mpv.playlistProxyModel.getPlayingItem()

                headerPositioning: ListView.OverlayHeader
                header: ToolBar {
                    id: toolbar

                    z: 100
                    width: parent.width

                    InputPopup {
                        id: addUrlPopup

                        width: toolbar.width - Kirigami.Units.largeSpacing
                        buttonText: i18nc("@action:button", "Add")

                        onUrlOpened: {
                            mpv.playlistModel.appendItem(url)
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
                                        fileDialog.fileMode = Platform.FileDialog.OpenFile
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
                                            fileDialog.fileMode = Platform.FileDialog.OpenFile
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
                                            mpv.playlistProxyModel.sortItems(PlayListProxyModel.NameAscending)
                                        }
                                    }
                                    Kirigami.Action {
                                        text: i18nc("@action:button", "Name descending")
                                        onTriggered: {
                                            mpv.playlistProxyModel.sortItems(PlayListProxyModel.NameDescending)
                                        }
                                    }
                                    Kirigami.Action {
                                        text: i18nc("@action:button", "Duration ascending")
                                        onTriggered: {
                                            mpv.playlistProxyModel.sortItems(PlayListProxyModel.DurationAscending)
                                        }
                                    }
                                    Kirigami.Action {
                                        text: i18nc("@action:button", "Duration descending")
                                        onTriggered: {
                                            mpv.playlistProxyModel.sortItems(PlayListProxyModel.DurationDescending)
                                        }
                                    }
                                },
                                Kirigami.Action {
                                    text: i18nc("@action:button", "Clear")
                                    icon.name: "edit-clear-all"
                                    displayHint: Kirigami.DisplayHint.AlwaysHide
                                    onTriggered: {
                                        mpv.playlistModel.clear()
                                    }
                                },
                                Kirigami.Action {
                                    text: i18nc("@action:button", "Save as")
                                    icon.name: "document-save-as"
                                    displayHint: Kirigami.DisplayHint.AlwaysHide
                                    onTriggered: {
                                        fileDialog.fileType = "playlist"
                                        fileDialog.fileMode = Platform.FileDialog.SaveFile
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
                        playListItemSimple
                        break
                    case "withThumbnails":
                        playListItemWithThumbnail
                        break
                    case "compact":
                        playListItemCompact
                        break
                    }
                }

                MouseArea {
                    anchors.fill: playlistView.contentItem
                    acceptedButtons: Qt.RightButton
                    onClicked: {
                        contextMenuLoader.row = playlistView.indexAt(mouseX, mouseY)
                        contextMenuLoader.isLocal = playlistView.itemAt(mouseX, mouseY).isLocal
                        contextMenuLoader.active = true
                        contextMenuLoader.item.popup(playlistView.itemAt(mouseX, mouseY))
                    }
                }

            }
        }

        Loader {
            id: contextMenuLoader

            property int row: -1
            property bool isLocal: false

            active: false
            sourceComponent: Menu {

                MenuItem {
                    text: i18nc("@action:inmenu", "Open containing folder")
                    icon.name: "folder"
                    visible: contextMenuLoader.isLocal
                    onClicked: mpv.playlistProxyModel.highlightInFileManager(row)
                }
                MenuItem {
                    text: i18nc("@action:inmenu", "Copy file name")
                    onClicked: mpv.playlistProxyModel.copyFileName(row)
                }
                MenuItem {
                    text: i18nc("@action:inmenu", "Copy file path")
                    onClicked: mpv.playlistProxyModel.copyFilePath(row)
                }
                MenuSeparator {}
                MenuItem {
                    text: i18nc("@action:inmenu", "Remove from playlist")
                    icon.name: "remove"
                    visible: contextMenuLoader.isLocal
                    onClicked: mpv.playlistProxyModel.removeItem(row)
                }
                MenuItem {
                    text: i18nc("@action:inmenu", "Rename file")
                    icon.name: "edit-rename"
                    visible: contextMenuLoader.isLocal
                    onClicked: mpv.playlistProxyModel.renameFile(row)
                }
                MenuItem {
                    text: i18nc("@action:inmenu", "Trash file")
                    icon.name: "delete"
                    visible: contextMenuLoader.isLocal && app.frameworksVersionMinor() >= 100
                    onClicked: mpv.playlistProxyModel.trashFile(row)
                }
            }
        }

        Component {
            id: playListItemWithThumbnail
            PlayListItemWithThumbnail {}
        }

        Component {
            id: playListItemSimple
            PlayListItem {}
        }

        Component {
            id: playListItemCompact
            PlayListItemCompact {}
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
            sourceItem: mpv
            sourceRect: PlaylistSettings.position === "right"
                        ? Qt.rect(mpv.width - root.width, mpv.y, root.width, root.height)
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

    Platform.FileDialog {
        id: fileDialog

        property url location: GeneralSettings.fileDialogLocation
                               ? app.pathToUrl(GeneralSettings.fileDialogLocation)
                               : app.pathToUrl(GeneralSettings.fileDialogLastLocation)
        property string fileType: "video"

        folder: location
        title: i18nc("@title:window", "Select file")
        fileMode: Platform.FileDialog.OpenFile

        onAccepted: {
            switch (fileType) {
            case "video":
                mpv.playlistModel.appendItem(fileDialog.file.toString())
                break
            case "playlist":
                if (fileMode === Platform.FileDialog.OpenFile) {
                    mpv.playlistModel.openM3uFile(fileDialog.file.toString())
                } else {
                    mpv.playlistProxyModel.saveM3uFile(fileDialog.file.toString())
                }

                break
            }
        }
        onRejected: mpv.focus = true
    }

    states: [
        State {
            name: "hidden"

            PropertyChanges {
                target: root;
                x: PlaylistSettings.position === "right" ? window.width : -width
            }

            PropertyChanges {
                target: root;
                visible: false
            }
        },
        State {
            name : "visible"

            PropertyChanges {
                target: root;
                x: PlaylistSettings.position === "right" ? window.width - root.width : 0
            }

            PropertyChanges {
                target: root;
                visible: true
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
