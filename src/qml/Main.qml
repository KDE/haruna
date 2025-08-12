/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

import org.kde.kirigami as Kirigami
import org.kde.config as KConfig

import org.kde.haruna
import org.kde.haruna.settings

ApplicationWindow {
    id: window

    property bool containsMouse: false

    property int previousVisibility: Window.Windowed
    property var acceptedSubtitleTypes: ["application/x-subrip", "text/x-ssa"]

    visible: true
    title: mpv.mediaTitle || i18nc("@title:window", "Haruna")
    width: 1000
    height: 600
    minimumWidth: 400
    minimumHeight: 200
    color: Kirigami.Theme.backgroundColor

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

    onClosing: {
        const settingsWindow = settingsLoader.item as Window
        settingsWindow?.close()
    }

    header: Header {
        id: header

        m_mpv: mpv
        m_menuBarLoader: menuBarLoader
        m_recentFilesModel: recentFilesModel
        m_settingsLoader: settingsLoader
    }

    menuBar: MenuBarLoader {
        id: menuBarLoader

        m_mpv: mpv
        m_recentFilesModel: recentFilesModel
        m_settingsLoader: settingsLoader
    }

    Connections {
        target: GeneralSettings
        function onOsdFontSizeChanged() {
            osd.message("Test osd font size")
        }
        function onMaxRecentFilesChanged() {
            recentFilesModel.getItems()
        }
        function onResizeWindowToVideoChanged() {
            window.resizeWindow()
        }
        function onColorSchemeChanged() {
            HarunaApp.activateColorScheme(GeneralSettings.colorScheme)
        }
    }
    Connections {
        target: GeneralSettings
        function onPreferredTrackChanged() {
            mpv.audioId = AudioSettings.preferredTrack === 0
                    ? "auto"
                    : AudioSettings.preferredTrack
        }
        function onPreferredLanguageChanged() {
            mpv.setProperty(MpvProperties.AudioLanguage, AudioSettings.preferredLanguage.replace(/\s+/g, ''))
        }
    }

    Loader {
        active: false
        sourceComponent: KConfig.WindowStateSaver {
            configGroupName: "MainWindow"
        }
        Component.onCompleted: active = GeneralSettings.rememberWindowGeometry
    }

    MpvVideo {
        id: mpv

        osd: osd
        mouseActionsModel: mouseActionsModel

        width: window.contentItem.width
        height: window.isFullScreen()
                ? window.contentItem.height
                : window.contentItem.height - (footer.isFloating ? 0 : footer.height)
        anchors.left: PlaylistSettings.overlayVideo
                      ? window.contentItem.left
                      : (PlaylistSettings.position === "left" ? playlist.right : window.contentItem.left)
        anchors.right: PlaylistSettings.overlayVideo
                       ? window.contentItem.right
                       : (PlaylistSettings.position === "right" ? playlist.left : window.contentItem.right)
        anchors.top: window.contentItem.top

        onVideoReconfig: {
            window.resizeWindow()
        }

        onAddToRecentFiles: function(url, openedFrom, name) {
            recentFilesModel.addRecentFile(url, openedFrom, name)
        }

        Osd {
            id: osd

            active: mpv.isReady
            maxWidth: mpv.width
        }

        SelectActionPopup {
            id: triggerActionPopup

            onActionSelected: function(actionName) {
                HarunaApp.actions[actionName].trigger()
            }
        }
    }

    PlayList {
        id: playlist

        m_mpv: mpv
        height: mpv.height

        Connections {
            target: mpv
            function onOpenPlaylist() {
                if (playlist.playlistView.count > 0) {
                    playlist.state = "visible"
                }
            }
            function onClosePlaylist() {
                playlist.state = "hidden"
            }
        }
    }

    Footer {
        id: footer

        anchors.bottom: window.contentItem.bottom

        m_mpv: mpv
        m_playlist: playlist
        m_menuBarLoader: menuBarLoader
        m_header: header
        m_recentFilesModel: recentFilesModel
        m_settingsLoader: settingsLoader
    }

    Actions {
        m_actionsModel: actionsModel
        m_mpv: mpv
        m_mpvContextMenuLoader: mpvContextMenuLoader
        m_osd: osd
        m_settingsLoader: settingsLoader
        m_triggerActionPopup: triggerActionPopup
        m_openUrlPopup: openUrlPopup

        onOpenFileDialog: fileDialog.open()
        onOpenSubtitleDialog: subtitlesFileDialog.open()
    }

    MouseActionsModel {
        id: mouseActionsModel
    }

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
        asynchronous: true
        sourceComponent: MpvContextMenu {
            m_mpv: mpv
            onClosed: mpvContextMenuLoader.active = false
        }

        function openMpvContextMenuLoader() : void {
            if (!mpvContextMenuLoader.active) {
                mpvContextMenuLoader.active = true
                mpvContextMenuLoader.loaded.connect(function() {
                    openMpvContextMenuLoader()
                })
                return
            }

            const contextMenu = mpvContextMenuLoader.item as MpvContextMenu
            contextMenu.popup()
        }

        function closeMpvContextMenuLoader() : void {
            mpvContextMenuLoader.active = false
        }
    }

    Loader {
        id: settingsLoader

        property int page: SettingsWindow.Page.General

        active: false
        asynchronous: true
        sourceComponent: SettingsWindow {
            m_mpv: mpv
            m_proxyActionsModel: proxyActionsModel
            m_customCommandsModel: customCommandsModel
            m_mouseActionsModel: mouseActionsModel

            onClosing: settingsLoader.active = false
            onCurrentPageChanged: settingsLoader.page = currentPage
        }

        function openSettingPage(page: int) : void {
            if (!settingsLoader.active) {
                settingsLoader.active = true
                settingsLoader.loaded.connect(function() {
                    settingsLoader.openSettingPage(page)
                })
                return
            }

            const settingsWindow = settingsLoader.item as SettingsWindow
            settingsWindow.currentPage = page
            if (settingsWindow.visible) {
                settingsWindow.raise()
            } else {
                settingsWindow.visible = true
            }
        }
    }

    Connections {
        target: HarunaApp
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
                let behavior = GeneralSettings.playNewFileInSingleInstance
                    ? PlaylistModel.AppendAndPlay
                    : PlaylistModel.Append

                mpv.playlistModel.addItem(url.toString(), behavior)
                return
            }

            window.openFile(url, RecentFilesModel.OpenedFrom.ExternalApp)
        }
    }

    FileDialog {
        id: fileDialog

        title: i18nc("@title:window", "Select File")
        currentFolder: HarunaApp.pathToUrl(GeneralSettings.fileDialogLastLocation)
        fileMode: FileDialog.OpenFile

        onAccepted: {
            window.openFile(fileDialog.selectedFile, RecentFilesModel.OpenedFrom.OpenAction)
            mpv.focus = true

            GeneralSettings.fileDialogLastLocation = HarunaApp.parentUrl(fileDialog.selectedFile)
            GeneralSettings.save()
        }
        onRejected: mpv.focus = true
    }

    FileDialog {
        id: subtitlesFileDialog

        title: i18nc("@title:window", "Select Subtitles File")
        currentFolder: HarunaApp.parentUrl(mpv.currentUrl)
        fileMode: FileDialog.OpenFile
        nameFilters: ["Subtitles (*.srt *.ssa *.ass)"]

        onAccepted: {
            if (window.acceptedSubtitleTypes.includes(HarunaApp.mimeType(subtitlesFileDialog.selectedFile))) {
                mpv.command(["sub-add", subtitlesFileDialog.selectedFile, "select"])
            }
        }
        onRejected: mpv.focus = true
    }

    YouTube {
        id: youtube
    }

    InputPopup {
        id: openUrlPopup

        x: 10
        y: 10
        width: Math.min(window.width * 0.9, 600)
        lastUrl: GeneralSettings.lastUrl
        buttonText: i18nc("@action:button", "Open")

        onUrlOpened: function(url) {
            window.openFile(youtube.normalizeUrl(url), RecentFilesModel.OpenedFrom.OpenAction)

            GeneralSettings.lastUrl = url
            GeneralSettings.save()
        }
    }

    Component.onCompleted: {
        HarunaApp.activateColorScheme(GeneralSettings.colorScheme)

        const hasCommandLineFile = HarunaApp.url(0).toString() !== ""
        const hasLastPlayedFile = GeneralSettings.lastPlayedFile !== ""
        const hasFileToOpen = hasCommandLineFile || (PlaybackSettings.openLastPlayedFile && hasLastPlayedFile)
        if (GeneralSettings.fullscreenOnStartUp && hasFileToOpen) {
            toggleFullScreen()
        }
    }

    function openFile(path: string, openedFrom: int) : void {
        recentFilesModel.addRecentFile(path, openedFrom)
        mpv.playlistModel.addItem(path, PlaylistModel.Clear)
    }

    function isFullScreen() : bool {
        return window.visibility === Window.FullScreen
    }

    function toggleFullScreen() : void {
        if (!isFullScreen()) {
            window.showFullScreen()
        } else {
            exitFullscreen()
        }
    }

    function exitFullscreen() : void {
        if (window.previousVisibility === Window.Maximized) {
            window.show()
            window.showMaximized()
        } else {
            window.showNormal()
        }
    }

    function resizeWindow() : void {
        if (HarunaApp.isPlatformWayland() || !GeneralSettings.resizeWindowToVideo || isFullScreen()) {
            return
        }

        window.width = mpv.videoWidth
        window.height = mpv.videoHeight
                + (footer.isFloating ? 0 : footer.height)
                + (header.visible ? header.height : 0)
                + (menuBar.visible ? menuBar.height : 0)
    }
}
