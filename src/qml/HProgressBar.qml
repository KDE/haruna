/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Shapes

import org.kde.kirigami as Kirigami
import org.kde.haruna
import org.kde.haruna.mpvproperties
import org.kde.haruna.settings

Slider {
    id: root

    required property MpvVideo m_mpv

    property alias loopIndicator: loopIndicator
    property bool seekStarted: false

    from: 0
    to: root.m_mpv.duration
    implicitWidth: 200
    implicitHeight: 25
    leftPadding: 0
    rightPadding: 0

    handle: Item { visible: false }

    background: Rectangle {
        id: progressBarBG
        color: Kirigami.Theme.alternateBackgroundColor
        scale: root.mirrored ? -1 : 1

        Rectangle {
            id: loopIndicator
            property double startPosition: -1
            property double endPosition: -1
            width: endPosition === -1 ? 1 : (endPosition / root.m_mpv.duration * progressBarBG.width) - x
            height: parent.height
            color: Qt.hsla(0, 0, 0, 0.4)
            visible: startPosition !== -1
            x: startPosition / root.m_mpv.duration * progressBarBG.width
            z: 110
        }

        Rectangle {
            width: root.position * parent.width
            height: parent.height
            color: Kirigami.Theme.highlightColor
        }

        ToolTip {
            id: progressBarToolTip

            z: 10
            visible: progressBarMouseArea.containsMouse && root.m_mpv.duration > 0
            timeout: -1
            delay: 0
            contentItem: ColumnLayout {

                Loader {
                    id: previewMpvLoader

                    property string file: root.m_mpv.currentUrl
                    property double position: 0
                    property double aspectRatio: 2

                    active: GeneralSettings.showPreviewThumbnail && previewMpvLoader.file !== "" && root.m_mpv.videoWidth > 0
                    visible: active && (item as MpvPreview).isLocalFile && (item as MpvPreview).isVideo
                    asynchronous: true
                    sourceComponent: MpvPreview {
                        id: mpvPreview
                        accuratePreview: GeneralSettings.accuratePreviewThumbnail
                        position: previewMpvLoader.position
                        file: previewMpvLoader.file

                        onAspectRatioChanged: previewMpvLoader.aspectRatio = aspectRatio || 2
                    }

                    Layout.preferredWidth: GeneralSettings.previewThumbnailWidth
                    Layout.preferredHeight: Math.ceil(Layout.preferredWidth / previewMpvLoader.aspectRatio)
                    Layout.alignment: Qt.AlignCenter

                }

                Label {
                    text: progressBarToolTip.text
                    Kirigami.Theme.colorSet: Kirigami.Theme.Tooltip
                    Layout.alignment: Qt.AlignCenter
                }
            }

        }

        MouseArea {
            id: progressBarMouseArea

            anchors.fill: parent
            hoverEnabled: true
            acceptedButtons: Qt.MiddleButton | Qt.RightButton

            onClicked: function(mouse) {
                if (mouse.button === Qt.MiddleButton) {
                    if (!GeneralSettings.showChapterMarkers) {
                        return
                    }

                    const time = mouseX / progressBarBG.width * root.to
                    const chapters = root.m_mpv.getProperty(MpvProperties.ChapterList)
                    const nextChapter = chapters.findIndex(chapter => chapter.time > time)
                    root.m_mpv.chapter = nextChapter
                }
                if (mouse.button === Qt.RightButton && root.m_mpv.chaptersModel.rowCount > 0) {
                    chaptersPopup.x = mouseX - chaptersPopup.width * 0.5
                    chaptersPopup.open()
                }
            }

            onMouseXChanged: {
                progressBarToolTip.x = mouseX - (progressBarToolTip.width * 0.5)

                const time = mouseX / progressBarBG.width * root.to
                previewMpvLoader.position = time
                progressBarToolTip.text = HarunaApp.formatTime(time)
            }

            onEntered: {
                progressBarToolTip.x = mouseX - (progressBarToolTip.width * 0.5)
                progressBarToolTip.y = root.height + Kirigami.Units.largeSpacing
            }

            onWheel: function(wheel) {
                if (wheel.angleDelta.y > 0) {
                    appActions.seekForwardMediumAction.trigger()
                } else if (wheel.angleDelta.y) {
                    appActions.seekBackwardMediumAction.trigger()
                }
            }
        }
    }

    onToChanged: value = root.m_mpv.position
    onPressedChanged: {
        if (pressed) {
            seekStarted = true
        } else {
            root.m_mpv.command(["seek", value, "absolute"])
            seekStarted = false
        }
    }

    // create markers for the chapters
    Repeater {
        id: chaptersInstantiator
        model: GeneralSettings.showChapterMarkers && root.m_mpv.chaptersModel.rowCount < 50
               ? root.m_mpv.chaptersModel
               : 0
        delegate: Shape {
            id: chapterMarkerShape

            required property int index
            required property string title
            required property double startTime

            // where the chapter marker shoud be positioned on the progress bar
            property int position: root.mirrored
                                   ? progressBarBG.width - (chapterMarkerShape.startTime / root.m_mpv.duration * progressBarBG.width)
                                   : chapterMarkerShape.startTime / root.m_mpv.duration * progressBarBG.width

            antialiasing: true
            ShapePath {
                id: shape
                strokeWidth: 1
                strokeColor: Kirigami.Theme.textColor
                startX: chapterMarkerShape.position
                startY: root.height
                fillColor: Kirigami.Theme.textColor
                PathLine { x: shape.startX; y: -1 }
                PathLine { x: shape.startX + 6; y: -7 }
                PathLine { x: shape.startX - 7; y: -7 }
                PathLine { x: shape.startX - 1; y: -1 }
            }
            Rectangle {
                x: chapterMarkerShape.position - 8
                y: -11
                width: 15
                height: 11
                color: "transparent"
                ToolTip {
                    id: chapterTitleToolTip
                    text: chapterMarkerShape.title
                    visible: false
                    delay: 0
                    timeout: 10000
                }
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: chapterTitleToolTip.visible = true
                    onExited: chapterTitleToolTip.visible = false
                    onClicked: root.m_mpv.chapter = chapterMarkerShape.index
                }
            }
        }
    }

    Popup {
        id: chaptersPopup

        property int itemHeight
        property int itemBiggestWidth: 1
        property var checkedItem
        property int maxWidth: root.Window.window.width * 0.7 > Kirigami.Units.gridUnit * 40
                               ? Kirigami.Units.gridUnit * 40
                               : root.Window.window.width * 0.7

        y: -height - root.height
        z: 20
        width: itemBiggestWidth > maxWidth ? maxWidth : itemBiggestWidth
        height: itemHeight * root.m_mpv.chaptersModel.rowCount + listViewPage.footer.height > root.m_mpv.height - Kirigami.Units.gridUnit
                ? root.m_mpv.height - Kirigami.Units.gridUnit - listViewPage.footer.height * 2
                : itemHeight * root.m_mpv.chaptersModel.rowCount + listViewPage.footer.height
        modal: true
        padding: 0
        onOpened: {
            listView.positionViewAtIndex(checkedItem, ListView.Beginning)
        }

        Kirigami.ScrollablePage {
            id: listViewPage

            padding: 0

            anchors.fill: parent
            footer: ToolBar {
                z: 100
                width: parent.width
                CheckBox {
                    text: i18nc("@action:inmenu", "Skip Chapters")
                    checked: PlaybackSettings.skipChapters
                    onCheckedChanged: {
                        PlaybackSettings.skipChapters = checked
                        PlaybackSettings.save()
                    }

                    ToolTip {
                        text: i18nc("@info:tooltip", "Automatically skips chapters containing certain words/characters.\nCheck 'Playback' settings for more details")
                    }
                }
            }

            ListView {
                id: listView

                model: root.m_mpv.chaptersModel
                reuseItems: true
                delegate: CheckDelegate {
                    id: menuitem

                    required property int index
                    required property string title
                    required property double startTime

                    property int scrollBarWidth: listViewPage.contentItem.ScrollBar.vertical.width

                    text: `${HarunaApp.formatTime(menuitem.startTime)} - ${menuitem.title}`
                    checked: menuitem.index === chaptersPopup.checkedItem
                    width: listViewPage.width - scrollBarWidth
                    onClicked: {
                        chaptersPopup.close()
                        root.m_mpv.chapter = menuitem.index
                    }
                    Component.onCompleted: {
                        chaptersPopup.itemBiggestWidth = menuitem.implicitWidth + scrollBarWidth > chaptersPopup.width
                                ? menuitem.implicitWidth + scrollBarWidth
                                : chaptersPopup.width

                        chaptersPopup.itemHeight = height
                    }
                }
            }
        }
    }

    Connections {
        target: root.m_mpv
        function onFileLoaded() {
            loopIndicator.startPosition = -1
            loopIndicator.endPosition = -1
            previewMpvLoader.file = root.m_mpv.currentUrl
        }
        function onChapterChanged() {
            chaptersPopup.checkedItem = root.m_mpv.chapter
        }
        function onPositionChanged() {
            if (!root.seekStarted) {
                root.value = root.m_mpv.position
            }
        }
    }
}
