/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import Qt.labs.platform 1.0 as Platform
import QtQml 2.12

import org.kde.kirigami 2.11 as Kirigami
import org.kde.haruna 1.0
import org.kde.haruna.models 1.0
import Haruna.Components 1.0

import "Menus"
import "Menus/Global"
import "Settings"

Kirigami.ApplicationWindow {
    id: window

    property bool containsMouse: false

    property int previousVisibility: Window.Windowed
    property var acceptedSubtitleTypes: ["application/x-subrip", "text/x-ssa"]
    property alias playList: playlistLoader.item

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

    onVisibilityChanged: {
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

    menuBar: Loader {
        id: menuBarLoader

        property bool showGlobalMenu: app.platformName() !== "windows"
                                      && Kirigami.Settings.hasPlatformMenuBar
                                      && !Kirigami.Settings.isMobile

        state: !window.isFullScreen() && GeneralSettings.showMenuBar ? "visible" : "hidden"
        sourceComponent: showGlobalMenu ? globalMenuBarComponent : menuBarComponent

        onVisibleChanged: {
            window.resizeWindow()
        }


        states: [
            State {
                name: "hidden"
                PropertyChanges {
                    target: menuBarLoader
                    height: 0
                    opacity: 0
                    visible: false
                }
            },
            State {
                name : "visible"
                PropertyChanges {
                    target: menuBarLoader
                    height: menuBarLoader.implicitHeight
                    opacity: 1
                    visible: true
                }
            }
        ]

        transitions: [
            Transition {
                from: "visible"
                to: "hidden"

                SequentialAnimation {
                    ParallelAnimation {
                        NumberAnimation {
                            target: menuBarLoader
                            property: "opacity"
                            duration: Kirigami.Units.veryShortDuration
                            easing.type: Easing.Linear
                        }
                        NumberAnimation {
                            target: menuBarLoader
                            property: "height"
                            duration: Kirigami.Units.veryShortDuration
                            easing.type: Easing.Linear
                        }
                    }
                    PropertyAction {
                        target: menuBarLoader
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
                        target: menuBarLoader
                        property: "visible"
                        value: true
                    }
                    ParallelAnimation {
                        NumberAnimation {
                            target: menuBarLoader
                            property: "height"
                            duration: Kirigami.Units.veryShortDuration
                            easing.type: Easing.Linear
                        }
                        NumberAnimation {
                            target: menuBarLoader
                            property: "opacity"
                            duration: Kirigami.Units.veryShortDuration
                            easing.type: Easing.Linear
                        }
                    }
                }
            }
        ]
    }

    MpvVideo {
        id: mpv

        width: window.contentItem.width
        height: window.isFullScreen() ? window.contentItem.height : window.contentItem.height - footer.height
        anchors.left: PlaylistSettings.overlayVideo
                      ? window.contentItem.left
                      : (PlaylistSettings.position === "left" ? playList.right : window.contentItem.left)
        anchors.right: PlaylistSettings.overlayVideo
                       ? window.contentItem.right
                       : (PlaylistSettings.position === "right" ? playList.left : window.contentItem.right)
        anchors.top: parent.top

        onVideoReconfig: {
            resizeWindow()
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
            anchors.centerIn: undefined
            width: Kirigami.Units.gridUnit * 20
            height: minHeight < Kirigami.Units.gridUnit * 16 ? maxHeight : minHeight
            title: ""
            subtitle: ""

            onActionSelected: {
                appActions[actionName].trigger()
            }
        }
    }

    Loader {
        id: playlistLoader

        active: true
        source: app.qtMajorVersion() === 6 ? "PlayListQt6.qml" : "PlayList.qml"
        onStatusChanged: {
            // hack to allow mpv to be anchored to the loader's item
            // otherwise mpv is not resized when PlaylistSettings.overlayVideo is disabled
            if (status === Loader.Ready) {
                item.parent = window.contentItem

                mpv.anchors.left = Qt.binding(
                            () => {
                                return (PlaylistSettings.overlayVideo
                                ? window.contentItem.left
                                : (PlaylistSettings.position === "left" ? playList.right : window.contentItem.left))
                            })
                mpv.anchors.right = Qt.binding(
                            () => {
                                return (PlaylistSettings.overlayVideo
                                ? window.contentItem.right
                                : (PlaylistSettings.position === "right" ? playList.left : window.contentItem.right))
                            })
            }
        }
    }

    Footer {
        id: footer

        m_mpv: mpv

        anchors.left: window.contentItem.left
        anchors.right: window.contentItem.right
        anchors.bottom: window.isFullScreen() ? mpv.bottom : window.contentItem.bottom
        state: !window.isFullScreen() || (mpv.mouseY > window.height - footer.implicitHeight && window.containsMouse)
               ? "visible" : "hidden"
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

    Component {
        id: menuBarComponent

        MenuBar {
            hoverEnabled: true
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

    Connections {
        target: app
        onQmlApplicationMouseLeave: {
            if (PlaylistSettings.canToggleWithMouse && window.isFullScreen()) {
                playList.state = "hidden"
            }
            window.containsMouse = false
        }
        onQmlApplicationMouseEnter: {
            window.containsMouse = true
        }
        onError: {
            messageBox.visible = true
            messageBox.text = message
        }
        onOpenUrl: {
            if (GeneralSettings.appendVideoToSingleInstance) {
                mpv.playlistModel.appendItem(url.toString())
            } else {
                openFile(url)
            }
        }
    }

    Platform.FileDialog {
        id: fileDialog

        property url location: GeneralSettings.fileDialogLocation
                               ? app.pathToUrl(GeneralSettings.fileDialogLocation)
                               : app.pathToUrl(GeneralSettings.fileDialogLastLocation)

        folder: location
        title: i18nc("@title:window", "Select file")
        fileMode: Platform.FileDialog.OpenFile

        onAccepted: {
            openFile(fileDialog.file.toString(), true)
            mpv.focus = true

            GeneralSettings.fileDialogLastLocation = app.parentUrl(fileDialog.file)
            GeneralSettings.save()
        }
        onRejected: mpv.focus = true
    }

    Platform.FileDialog {
        id: subtitlesFileDialog

        property url location: {
            if (mpv.playlistModel.length > 0) {
                const item = mpv.playlistModel.getItem(mpv.playlistModel.getPlayingItem())
                return app.pathToUrl(item.folderPath())
            } else {
                return (GeneralSettings.fileDialogLocation
                ? app.pathToUrl(GeneralSettings.fileDialogLocation)
                : app.pathToUrl(GeneralSettings.fileDialogLastLocation))
            }
        }

        folder: location
        title: i18nc("@title:window", "Select subtitles file")
        fileMode: Platform.FileDialog.OpenFile
        nameFilters: ["Subtitles (*.srt *.ssa *.ass)"]

        onAccepted: {
            if (acceptedSubtitleTypes.includes(app.mimeType(subtitlesFileDialog.file))) {
                mpv.commandAsync(["sub-add", subtitlesFileDialog.file.toString(), "select"])
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

        onUrlOpened: {
            window.openFile(url)
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
        app.restoreWindowGeometry(window)
        app.activateColorScheme(GeneralSettings.colorScheme)
    }

    function openFile(path, addToRecentFiles = false) {
        mpv.playlistModel.openFile(path)
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
        if (!GeneralSettings.resizeWindowToVideo || isFullScreen()) {
            return
        }

        window.width = mpv.videoWidth
        window.height = mpv.videoHeight
                + (footer.visible ? footer.height : 0)
                + (header.visible ? header.height : 0)
                + (menuBar.visible ? menuBar.height : 0)
    }
}
