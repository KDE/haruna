/*
 * SPDX-FileCopyrightText: 2026 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls

import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.haruna
import org.kde.haruna.utilities
import org.kde.haruna.settings

Menu {
    id: root

    required property int index
    required property bool isLocal
    required property PlaylistsManager playlistsManager

    MenuItem {
        text: KI18n.i18nc("@action:inmenu", "Open Containing Folder")
        icon.name: "folder"
        visible: root.isLocal
        onClicked: root.playlistsManager.visiblePlaylist.highlightInFileManager(root.index)
    }

    MenuItem {
        text: KI18n.i18nc("@action:inmenu", "Open in Browser")
        icon.name: "link"
        visible: !root.isLocal && root.index != -1
        onClicked: {
            const playlist = root.playlistsManager.visiblePlaylist
            const modelIndex = playlist.index(root.index, 0)
            const url = playlist.data(modelIndex, PlaylistModel.PathRole)
            Qt.openUrlExternally(url)
        }
    }

    MenuItem {
        text: KI18n.i18nc("@action:inmenu", "Open in Thumbnail Generator")
        icon.name: "hana"
        visible: {
            const playlist = root.playlistsManager.visiblePlaylist
            const modelIndex = playlist.index(root.index, 0)
            return playlist.data(modelIndex, PlaylistModel.TypeRole) === "video"
                    && SystemUtils.isHanaInstalled()
                    && SystemUtils.platformName() !== "windows"
                    && root.isLocal && root.index != -1
        }
        onClicked: {
            const playlist = root.playlistsManager.visiblePlaylist
            const modelIndex = playlist.index(root.index, 0)
            const url = playlist.data(modelIndex, PlaylistModel.PathRole)
            SystemUtils.openHana(url)
        }
    }

    MenuItem {
        text: KI18n.i18nc("@action:inmenu %1 is 'MediaInfo' (app name)", "Open in %1", "MediaInfo")
        visible: SystemUtils.isMediaInfoInstalled()
                 && SystemUtils.platformName() !== "windows"
                 && root.isLocal && root.index != -1
        onClicked: {
            const playlist = root.playlistsManager.visiblePlaylist
            const modelIndex = playlist.index(root.index, 0)
            const url = playlist.data(modelIndex, PlaylistModel.PathRole)
            SystemUtils.openMediaInfo(url)
        }
    }

    MenuItem {
        text: KI18n.i18nc("@action:inmenu", "Copy Name")
        onClicked: root.playlistsManager.visiblePlaylist.copyFileName(root.index)
        visible: root.index != -1
    }

    MenuItem {
        text: root.isLocal
              ? KI18n.i18nc("@action:inmenu", "Copy Path")
              : KI18n.i18nc("@action:inmenu", "Copy URL")
        onClicked: root.playlistsManager.visiblePlaylist.copyFilePath(root.index)
        visible: root.index != -1
    }

    MenuItem {
        id: updateMetadataMenuItem

        text: KI18n.i18nc("@action:inmenu", "Update metadata")
        icon.name: "view-refresh"
        visible: root.index != -1
        onClicked: {
            const playlist = root.playlistsManager.visiblePlaylist
            const modelIndex = playlist.index(root.index, 0)
            const url = playlist.data(modelIndex, PlaylistModel.PathRole)
            Database.updateMetadata(url)
        }

        ToolTip {
            text: KI18n.i18nc("@info:tooltip", "Update metadata in the database with metadata inside the file.\n" +
                              "Metadata is stored in the database for faster retrieval.")
            delay: HarunaApp.isAltKeyPressed ? 0 : Kirigami.Units.toolTipDelay
            visible: updateMetadataMenuItem.hovered
                     && (GeneralSettings.showExplanatoryToolTips || HarunaApp.isAltKeyPressed)
        }
    }

    MenuSeparator {
        visible: root.index != -1
    }

    // Selection manipulators
    MenuItem {
        text: KI18n.i18nc("@action:inmenu", "Select All")
        onClicked: root.playlistsManager.visiblePlaylist.selectItem(0, PlaylistFilterProxyModel.All)
    }

    MenuItem {
        text: KI18n.i18nc("@action:inmenu", "Deselect All")
        onClicked: root.playlistsManager.visiblePlaylist.selectItem(0, PlaylistFilterProxyModel.Clear)
    }

    MenuItem {
        text: KI18n.i18nc("@action:inmenu", "Invert Selection")
        onClicked: root.playlistsManager.visiblePlaylist.selectItem(0, PlaylistFilterProxyModel.Invert)
    }

    MenuSeparator {}

    MenuItem {
        text: KI18n.i18nc("@action:inmenu", "Remove from Playlist")
        icon.name: "remove"
        onClicked: root.playlistsManager.visiblePlaylist.removeItem(root.index)
        visible: root.playlistsManager.visiblePlaylist.selectionCount === 1 && root.index != -1
    }

    MenuItem {
        text: KI18n.i18nc("@action:inmenu", "Remove Selected from Playlist")
        icon.name: "remove"
        onClicked: root.playlistsManager.visiblePlaylist.removeItems()
        visible: root.playlistsManager.visiblePlaylist.selectionCount > 1 && root.index != -1
    }

    MenuItem {
        text: KI18n.i18nc("@action:inmenu", "Rename")
        icon.name: "edit-rename"
        visible: root.isLocal && root.index != -1
        onClicked: root.playlistsManager.visiblePlaylist.renameFile(root.index)
    }

    MenuItem {
        text: KI18n.i18nc("@action:inmenu", "Scroll to Playing Item")
        onClicked: {
            const index = root.playlistsManager.visiblePlaylist.getPlayingItem()
            root.ListView.view.positionViewAtIndex(index, ListView.Beginning)
        }
    }

    MenuSeparator {
        visible: root.isLocal && root.index != -1
    }

    MenuItem {
        text: KI18n.i18nc("@action:inmenu", "Move File to Trash")
        icon.name: "delete"
        visible: root.isLocal && root.playlistsManager.visiblePlaylist.selectionCount === 1 && root.index != -1
        onClicked: root.playlistsManager.visiblePlaylist.trashFile(root.index)
    }

    MenuItem {
        text: KI18n.i18nc("@action:inmenu", "Move Selected Files to Trash")
        icon.name: "delete"
        visible: root.isLocal && root.playlistsManager.visiblePlaylist.selectionCount > 1 && root.index != -1
        onClicked: root.playlistsManager.visiblePlaylist.trashFiles()
    }
}