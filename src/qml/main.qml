/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.12
import Qt.labs.platform 1.0 as Platform
import QtQml 2.12

import org.kde.kirigami 2.11 as Kirigami
import org.kde.haruna 1.0

import "Menus"
import "Menus/Global"
import "Settings"

Kirigami.ApplicationWindow {
    id: window

    property int previousVisibility: Window.Windowed

    visible: true
    title: mpv.mediaTitle || i18n("Haruna")
    width: 1200
    minimumWidth: 700
    height: 720
    minimumHeight: 450
    color: Kirigami.Theme.backgroundColor

    onClosing: app.saveWindowGeometry(window)
    onWidthChanged: saveWindowGeometryTimer.restart()
    onHeightChanged: saveWindowGeometryTimer.restart()
    onXChanged: saveWindowGeometryTimer.restart()
    onYChanged: saveWindowGeometryTimer.restart()

    onVisibilityChanged: {
        if (PlaybackSettings.pauseWhileMinimized) {
            if (visibility === Window.Minimized) {
                mpv.pause = true
            }
            if (previousVisibility === Window.Minimized
                    && visibility === Window.Windowed | Window.Maximized | Window.FullScreen) {
                mpv.pause = false
            }
        }

        previousVisibility = visibility
    }

    header: Header { id: header }

    menuBar: Loader {
        id: menuBarLoader

        property bool showGlobalMenu: Kirigami.Settings.hasPlatformMenuBar && !Kirigami.Settings.isMobile

        visible: !window.isFullScreen() && GeneralSettings.showMenuBar
        sourceComponent: showGlobalMenu ? globalMenuBarComponent : menuBarComponent
    }

    Component {
        id: menuBarComponent

        MenuBar {
            id: menuBar
            hoverEnabled: true
            background: Rectangle {
                color: Kirigami.Theme.backgroundColor
            }
            Kirigami.Theme.colorSet: Kirigami.Theme.Header

            FileMenu {}
            ViewMenu {}
            PlaybackMenu {}
            VideoMenu {}
            SubtitlesMenu {}
            AudioMenu {}
            SettingsMenu {}
            HelpMenu {}
        }
    }

    Component {
        id: globalMenuBarComponent

        Platform.MenuBar {
            GlobalFileMenu {}
            GlobalViewMenu {}
            GlobalPlaybackMenu {}
            GlobalVideoMenu {}
            GlobalSubtitlesMenu {}
            GlobalAudioMenu {}
            GlobalSettingsMenu {}
            GlobalHelpMenu {}
        }
    }

    Loader {
        id: mpvContextMenuLoader

        active: false
        sourceComponent: ContextMenu {
            onClosed: mpvContextMenuLoader.active = false
        }
    }

    HamburgerMenu { id: hamburgerMenu }

    SystemPalette { id: systemPalette; colorGroup: SystemPalette.Active }

    Loader {
        id: settingsLoader

        active: false
        sourceComponent: SettingsWindow {}
    }

    MpvVideo {
        id: mpv

        Osd { id: osd }
    }

    PlayList { id: playList }

    Footer { id: footer }

    Actions {}

    RowLayout {
        width: window.width * 0.8 > Kirigami.Units.gridUnit * 50
               ? Kirigami.Units.gridUnit * 50
               : window.width * 0.8
        anchors.centerIn: parent

        Kirigami.InlineMessage {
            id: messageBox

            Layout.fillWidth: true
            Layout.fillHeight: true
            type: Kirigami.MessageType.Error
            showCloseButton: true
        }
    }

    Connections {
        target: app
        onQmlApplicationMouseLeave: {
            if (PlaylistSettings.canToggleWithMouse && window.isFullScreen()) {
                playList.state = "hidden"
            }
        }
        onError: {
            messageBox.visible = true
            messageBox.text = message
        }
    }

    Platform.FileDialog {
        id: fileDialog

        property url location: GeneralSettings.fileDialogLocation
                               ? app.pathToUrl(GeneralSettings.fileDialogLocation)
                               : app.pathToUrl(GeneralSettings.fileDialogLastLocation)

        folder: location
        title: i18n("Select file")
        fileMode: Platform.FileDialog.OpenFile

        onAccepted: {
            openFile(fileDialog.file.toString(), true, PlaylistSettings.loadSiblings, true)
            // the timer scrolls the playlist to the playing file
            // once the table view rows are loaded
            playList.scrollPositionTimer.start()
            mpv.focus = true

            GeneralSettings.fileDialogLastLocation = app.parentUrl(fileDialog.file)
            GeneralSettings.save()
        }
        onRejected: mpv.focus = true
    }

    Popup {
        id: openUrlPopup
        x: 10
        y: 10

        onOpened: {
            openUrlTextField.forceActiveFocus(Qt.MouseFocusReason)
            openUrlTextField.selectAll()
        }

        RowLayout {
            anchors.fill: parent

            Label {
                text: i18n("<a href=\"https://youtube-dl.org\">Youtube-dl</a> was not found.")
                visible: !app.hasYoutubeDl()
                onLinkActivated: Qt.openUrlExternally(link)
            }

            TextField {
                id: openUrlTextField

                visible: app.hasYoutubeDl()
                Layout.preferredWidth: 400
                Layout.fillWidth: true
                Component.onCompleted: text = GeneralSettings.lastUrl

                Keys.onPressed: {
                    if (event.key === Qt.Key_Enter
                            || event.key === Qt.Key_Return) {
                        openUrlButton.clicked()
                    }
                    if (event.key === Qt.Key_Escape) {
                        openUrlPopup.close()
                    }
                }
            }
            Button {
                id: openUrlButton

                visible: app.hasYoutubeDl()
                text: i18n("Open")

                onClicked: {
                    openFile(openUrlTextField.text, true, false, true)
                    GeneralSettings.lastUrl = openUrlTextField.text
                    // in case the url is a playList, it opens the first video
                    GeneralSettings.lastPlaylistIndex = 0
                    openUrlPopup.close()
                    openUrlTextField.clear()
                }
            }
        }
    }

    // This timer allows to batch update the window size change to reduce
    // the io load and also work around the fact that x/y/width/height are
    // changed when loading the page and overwrite the saved geometry from
    // the previous session.
    Timer {
        id: saveWindowGeometryTimer
        interval: 1000
        onTriggered: app.saveWindowGeometry(window)
    }

    Component.onCompleted: {
        app.restoreWindowGeometry(window)
        app.activateColorScheme(GeneralSettings.colorScheme)
    }

    function openFile(path, startPlayback, loadSiblings, addToRecentFiles = false) {

        if (app.isYoutubePlaylist(path)) {
            mpv.getYouTubePlaylist(path);
            playList.isYouTubePlaylist = true
        } else {
            playList.isYouTubePlaylist = false
        }

        if (addToRecentFiles && GeneralSettings.maxRecentFiles > 0) {
            if (!playList.isYouTubePlaylist) {
                recentFilesModel.addUrl(path)
            }
        }

        mpv.playlistModel.clear()
        mpv.pause = !startPlayback
        if (loadSiblings) {
            // get video files from same folder as the opened file
            mpv.playlistModel.getVideos(path)
        } else {
            mpv.playlistModel.appendVideo(path)
        }

        mpv.loadFile(path)
    }

    function isFullScreen() {
        return window.visibility === Window.FullScreen
    }

    function toggleFullScreen() {
        if (!isFullScreen()) {
            window.showFullScreen()
        } else {
            exitFullscreen()
        }
        playList.scrollPositionTimer.start()
    }

    function exitFullscreen() {
        if (window.previousVisibility === Window.Maximized) {
            window.show()
            window.showMaximized()
        } else {
            window.showNormal()
        }
    }
}
