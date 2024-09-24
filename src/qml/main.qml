/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import Qt5Compat.GraphicalEffects
import Qt.labs.platform 1.0 as Platform
import org.kde.kirigami 2.11 as Kirigami
import AppSettings 1.0

import mpv 1.0
import "Menus"
import "Settings"

Kirigami.ApplicationWindow {
    id: window

    property var configure: app.action("configure")
    property int preFullScreenVisibility

    visible: true
    title: mpv.mediaTitle || qsTr("Haruna")
    width: 1200
    minimumWidth: 700
    height: 720
    minimumHeight: 450
    color: Kirigami.Theme.backgroundColor

    onVisibilityChanged: function(visibility) {
        if (!window.isFullScreen()) {
            preFullScreenVisibility = visibility
        }
    }

    header: Header { id: header }

    menuBar: MenuBar {

        hoverEnabled: true
        implicitHeight: 24
        visible: !window.isFullScreen() && AppSettings.viewIsMenuBarVisible

        FileMenu {}
        ViewMenu {}
        PlaybackMenu {}
        SubtitlesMenu {}
        AudioMenu {}
        SettingsMenu {}
    }

    Menu {
        id: mpvContextMenu

        modal: true

        FileMenu {}
        ViewMenu {}
        PlaybackMenu {}
        SubtitlesMenu {}
        AudioMenu {}
        SettingsMenu {}
    }

    Actions { id: actions }

    SystemPalette { id: systemPalette; colorGroup: SystemPalette.Active }

    SettingsEditor { id: settingsEditor }

    MpvVideo {
        id: mpv

        Osd { id: osd }
    }

    PlayList { id: playList }

    Footer { id: footer }

    Platform.FileDialog {
        id: fileDialog
        folder: Platform.StandardPaths.writableLocation(Platform.StandardPaths.MoviesLocation)
        title: "Select file"
        fileMode: Platform.FileDialog.OpenFile

        onAccepted: {
            openFile(fileDialog.file.toString(), true, true)
            // the timer scrolls the playlist to the playing file
            // once the table view rows are loaded
            mpv.scrollPositionTimer.start()
            mpv.focus = true
        }
        onRejected: mpv.focus = true
    }

    Popup {
        id: openUrlPopup
        width: 500
        x: 10
        y: 10

        onOpened: {
            openUrlTextField.forceActiveFocus(Qt.MouseFocusReason)
            openUrlTextField.selectAll()
        }

        RowLayout {
            anchors.fill: parent
            TextField {
                id: openUrlTextField
                Layout.fillWidth: true
                Component.onCompleted: text = AppSettings.lastUrl

                Keys.onPressed: {
                    if (event.key === Qt.Key_Enter || event.key === Qt.Key_Return) {
                        openFile(openUrlTextField.text, true, false)
                        AppSettings.lastUrl = openUrlTextField.text
                        openUrlPopup.close()
                        openUrlTextField.clear()
                        // clear playlist to prevent existing files in the playlist
                        // to be loaded when playback ends
                        playList.tableView.model = 0
                    }
                    if (event.key === Qt.Key_Escape) {
                        openUrlPopup.close()
                    }
                }
            }
            Button {
                id: openUrlButton
                text: qsTr("Open")

                onClicked: {
                    openFile(openUrlTextField.text, true, false)
                    AppSettings.lastUrl = openUrlTextField.text
                    openUrlPopup.close()
                    openUrlTextField.clear()
                    playList.tableView.model = 0
                }
            }
        }
    }

    function openFile(path, startPlayback, loadSiblings) {
        mpv.setProperty("ytdl-format", AppSettings.playbackYtdlFormat)
        mpv.command(["loadfile", path])
        mpv.setProperty("pause", !startPlayback)
        if (loadSiblings) {
            // get video files from same folder as the opened file
            playListModel.getVideos(path)
        }

        AppSettings.lastPlayedFile = path
    }

    function isFullScreen() {
        return window.visibility === Window.FullScreen
    }
}
