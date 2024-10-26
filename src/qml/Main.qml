/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQml

import org.kde.kirigami as Kirigami
import org.kde.haruna
import org.kde.haruna.settings

pragma ComponentBehavior: Bound

Kirigami.ApplicationWindow {
    id: window

    property bool containsMouse: false

    property int previousVisibility: Window.Windowed
    property var acceptedSubtitleTypes: ["application/x-subrip", "text/x-ssa"]

    visible: true
    title: mpv.mediaTitle || i18nc("@title:window", "Haruna")
    width: Kirigami.Units.gridUnit * 66
    minimumWidth: Kirigami.Units.gridUnit * 36
    height: Kirigami.Units.gridUnit * 40
    minimumHeight: Kirigami.Units.gridUnit * 22
    color: Kirigami.Theme.backgroundColor

    onClosing: app.saveWindowGeometry(window)
    onWidthChanged: saveWindowGeometryTimer.restart()
    onHeightChanged: saveWindowGeometryTimer.restart()
    onXChanged: saveWindowGeometryTimer.restart()
    onYChanged: saveWindowGeometryTimer.restart()

    onVisibilityChanged: function(visibility) {
        if (PlaybackSettings.pauseWhileMinimized) {
            if (visibility === Window.Minimized) {
                if (mpv.pause) {
                    mpv.preMinimizePlaybackState = MpvVideo.PlaybackState.Paused
                } else {
                    mpv.preMinimizePlaybackState = MpvVideo.PlaybackState.Playing
                }
                mpv.pause = true
            }
            if (previousVisibility === Window.Minimized
                    && visibility === Window.Windowed | Window.Maximized | Window.FullScreen) {
                if (mpv.preMinimizePlaybackState === MpvVideo.PlaybackState.Playing) {
                    mpv.pause = false
                }
            }
        }

        // used to restore window state, when exiting fullscreen,
        // to the one it had before going fullscreen
        if (visibility !== Window.FullScreen) {
            previousVisibility = visibility
        }
    }

    header: Header { id: header }

    menuBar: MenuBarLoader {
        id: menuBarLoader
    }

    MpvVideo {
        id: mpv

        width: window.contentItem.width
        height: window.isFullScreen()
                ? window.contentItem.height
                : window.contentItem.height - footerLoader.footerHeight
        anchors.left: PlaylistSettings.overlayVideo
                      ? window.contentItem.left
                      : (PlaylistSettings.position === "left" ? playlist.right : window.contentItem.left)
        anchors.right: PlaylistSettings.overlayVideo
                       ? window.contentItem.right
                       : (PlaylistSettings.position === "right" ? playlist.left : window.contentItem.right)
        anchors.top: parent.top

        onVideoReconfig: {
            resizeWindow()
        }

        onAddToRecentFiles: function(url) {
            recentFilesModel.addUrl(url)
        }

        Osd {
            id: osd

            maxWidth: mpv.width
        }

        SelectActionPopup {
            id: triggerActionPopup

            property int minHeight: mpv.height * 0.5
            property int maxHeight: mpv.height * 0.9

            x: mpv.width * 0.5 - width * 0.5
            y: Kirigami.Units.largeSpacing
            width: Kirigami.Units.gridUnit * 20
            height: minHeight < Kirigami.Units.gridUnit * 16 ? maxHeight : minHeight
            title: ""
            subtitle: ""

            onActionSelected: function (actionName) {
                appActions[actionName].trigger()
            }
        }
    }

    PlayList {
        id: playlist
        height: window.isFullScreen() ? mpv.height - footerLoader.footerHeight : mpv.height
    }

    Loader {
        id: footerLoader

        property int footerHeight: item.isFloating ? 0 : item.height

        active: false
        anchors.bottom: window.contentItem.bottom
        sourceComponent: GeneralSettings.footerStyle === "default"
                         ? footerComponent
                         : floatingFooterComponent
    }

    Component {
        id: footerComponent

        Footer {
            m_window: window
            m_mpv: mpv
            m_playlist: playlist
        }
    }

    Component {
        id: floatingFooterComponent

        FloatingFooter {
            m_window: window
            m_mpv: mpv
            m_playlist: playlist
        }
    }

    Actions {}

    ActionsModel {
        id: actionsModel
    }

    ProxyActionsModel {
        id: proxyActionsModel

        sourceModel: actionsModel
    }

    CustomCommandsModel {
        id: customCommandsModel

        appActionsModel: actionsModel
        Component.onCompleted: init()
    }

    RecentFilesModel {
        id: recentFilesModel
    }

    SubtitlesFoldersModel {
        id: subtitlesFoldersModel
    }

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

    Loader {
        id: mpvContextMenuLoader

        active: false
        sourceComponent: ContextMenu {
            onClosed: mpvContextMenuLoader.active = false
        }
    }

    Loader {
        id: settingsLoader

        active: false
        sourceComponent: SettingsWindow {}
    }

    Connections {
        target: app
        function onQmlApplicationMouseLeave() {
            if (PlaylistSettings.canToggleWithMouse && window.isFullScreen()) {
                playlist.state = "hidden"
            }
            window.containsMouse = false
        }
        function onQmlApplicationMouseEnter() {
            window.containsMouse = true
        }
        function onError(message) {
            messageBox.visible = true
            messageBox.text = message
        }
        function onOpenUrl(url) {
            if (GeneralSettings.appendVideoToSingleInstance) {
                mpv.playlistModel.addItem(url.toString(), PlaylistModel.Append)
            } else {
                openFile(url)
            }
        }
    }

    FileDialog {
        id: fileDialog

        property url location: GeneralSettings.fileDialogLocation
                               ? app.pathToUrl(GeneralSettings.fileDialogLocation)
                               : app.pathToUrl(GeneralSettings.fileDialogLastLocation)

        title: i18nc("@title:window", "Select file")
        currentFolder: location
        fileMode: FileDialog.OpenFile

        onAccepted: {
            openFile(fileDialog.selectedFile, true)
            mpv.focus = true

            GeneralSettings.fileDialogLastLocation = app.parentUrl(fileDialog.selectedFile)
            GeneralSettings.save()
        }
        onRejected: mpv.focus = true
    }

    FileDialog {
        id: subtitlesFileDialog

        property url location: {
            if (mpv.currentUrl) {
                return app.parentUrl(mpv.currentUrl)
            } else {
                return (GeneralSettings.fileDialogLocation
                ? app.pathToUrl(GeneralSettings.fileDialogLocation)
                : app.pathToUrl(GeneralSettings.fileDialogLastLocation))
            }
        }

        title: i18nc("@title:window", "Select subtitles file")
        currentFolder: location
        fileMode: FileDialog.OpenFile
        nameFilters: ["Subtitles (*.srt *.ssa *.ass)"]

        onAccepted: {
            if (acceptedSubtitleTypes.includes(app.mimeType(subtitlesFileDialog.selectedFile))) {
                mpv.command(["sub-add", subtitlesFileDialog.selectedFile, "select"])
            }
        }
        onRejected: mpv.focus = true
    }

    InputPopup {
        id: openUrlPopup

        x: 10
        y: 10
        lastUrl: GeneralSettings.lastUrl
        buttonText: i18nc("@action:button", "Open")

        onUrlOpened: function(url) {
            window.openFile(url, true)
            GeneralSettings.lastUrl = url
            GeneralSettings.save()
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
        footerLoader.active = true
        app.restoreWindowGeometry(window)
        app.activateColorScheme(GeneralSettings.colorScheme)

        if (GeneralSettings.fullscreenOnStartUp) {
            toggleFullScreen()
        }
    }

    function openFile(path, addToRecentFiles = false) {
        if (addToRecentFiles) {
            recentFilesModel.addUrl(path)
        }

        mpv.playlistModel.addItem(path, PlaylistModel.Clear)
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
    }

    function exitFullscreen() {
        if (window.previousVisibility === Window.Maximized) {
            window.show()
            window.showMaximized()
        } else {
            window.showNormal()
        }
    }

    function resizeWindow() {
        if (app.isPlatformWayland() || !GeneralSettings.resizeWindowToVideo || isFullScreen()) {
            return
        }

        window.width = mpv.videoWidth
        window.height = mpv.videoHeight
                + (footerLoader.visible ? footerLoader.height : 0)
                + (header.visible ? header.height : 0)
                + (menuBar.visible ? menuBar.height : 0)
    }
}
