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
import org.kde.haruna.utilities
import org.kde.haruna.settings

RowLayout {
    id: root

    required property MpvVideo m_mpv

    property alias loopIndicator: loopIndicator
    property alias chaptersPopupIsOpen: chaptersPopup.visible
    property bool showChapterMarkers: GeneralSettings.showChapterMarkers
                                      && m_mpv.chaptersModel.rowCount * 20 < width

    function openChapterPopup(triggerItem: Item) {
        const x = (chaptersPopup.width + triggerItem.width) * -0.5
        const globalPoint = root.mapToGlobal(x, 0)
        const localPoint = root.mapFromGlobal(Kirigami.Units.largeSpacing, 0)
        chaptersPopup.x = globalPoint.x < 0 ? localPoint.x : x
        chaptersPopup.open()
    }

    spacing: Kirigami.Units.smallSpacing

    Slider {
        id: slider

        property bool seekStarted: false
        property bool prePressPause: root.m_mpv.pause

        from: 0
        to: root.m_mpv.duration
        implicitWidth: root.width
        implicitHeight: root.height
        leftPadding: 0
        rightPadding: 0

        Layout.fillWidth: true

        handle: Item { visible: false }

        background: Rectangle {
            id: progressBarBG
            color: Kirigami.Theme.alternateBackgroundColor
            scale: slider.mirrored ? -1 : 1

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
                width: slider.position * parent.width
                height: parent.height
                color: Kirigami.Theme.highlightColor
            }

            ToolTip {
                id: progressBarToolTip

                x: {
                    if (slider.pressed) {
                        const xPos = (slider.value * 100 / slider.to) / 100 * slider.width
                        return xPos - (progressBarToolTip.width * 0.5)
                    }

                    return progressBarMouseArea.mouseX - (progressBarToolTip.width * 0.5)
                }
                y: -height - Kirigami.Units.largeSpacing
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

                        const time = mouseX / progressBarBG.width * slider.to
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
                    const time = mouseX / progressBarBG.width * slider.to
                    previewMpvLoader.position = time
                    progressBarToolTip.text = MiscUtils.formatTime(time)
                }

                onWheel: function(wheel) {
                    if (wheel.angleDelta.y > 0) {
                        HarunaApp.actions.seekForwardMediumAction.trigger()
                    } else if (wheel.angleDelta.y) {
                        HarunaApp.actions.seekBackwardMediumAction.trigger()
                    }
                }
            }
        }

        onToChanged: value = root.m_mpv.position

        onMoved: {
            if (!pressed) {
                return
            }

            root.m_mpv.commandAsync(["seek", slider.value, "absolute"])
            progressBarToolTip.text = MiscUtils.formatTime(slider.value)

            const previewItem = previewMpvLoader.item as MpvPreview
            if (previewItem === null || !previewItem.isLocalFile) {
                return
            }
            previewItem.commandAsync(["seek", slider.value, "absolute"])
        }

        onPressedChanged: {
            if (pressed) {
                slider.prePressPause = root.m_mpv.getProperty(MpvProperties.Pause)
                root.m_mpv.setPropertyBlocking(MpvProperties.Pause, true)
            } else {
                root.m_mpv.setPropertyBlocking(MpvProperties.Pause, slider.prePressPause)
            }
        }

        // create markers for the chapters
        Repeater {
            id: chaptersInstantiator
            model: root.showChapterMarkers
                   ? root.m_mpv.chaptersModel
                   : 0
            delegate: Shape {
                id: chapterMarkerShape

                required property int index
                required property string title
                required property double startTime

                // where the chapter marker should be positioned on the progress bar
                property int position: slider.mirrored
                                       ? progressBarBG.width - (chapterMarkerShape.startTime / root.m_mpv.duration * progressBarBG.width)
                                       : chapterMarkerShape.startTime / root.m_mpv.duration * progressBarBG.width

                antialiasing: true
                ShapePath {
                    id: shape
                    strokeWidth: 1
                    strokeColor: Kirigami.Theme.textColor
                    startX: chapterMarkerShape.position
                    startY: slider.height
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
            property int maxWidth: Math.max(Kirigami.Units.gridUnit * 40, root.width * 0.8)

            y: -height - slider.height
            z: 20
            width: Math.min(itemBiggestWidth, maxWidth)
            height: Math.min(root.m_mpv.height - Kirigami.Units.gridUnit * 2,
                             listViewPage.contentHeight + listViewPage.footer.height)
            modal: true
            padding: 0
            onOpened: {
                listView.positionViewAtIndex(root.m_mpv.chapter, ListView.Beginning)
            }

            Kirigami.ScrollablePage {
                id: listViewPage

                padding: 0

                anchors.fill: parent
                footer: ToolBar {
                    z: 100
                    width: parent.width
                    CheckBox {
                        anchors.verticalCenter: parent.verticalCenter
                        text: i18nc("@action:inmenu", "Skip Chapters")
                        checked: PlaybackSettings.skipChapters
                        onCheckedChanged: {
                            PlaybackSettings.skipChapters = checked
                            PlaybackSettings.save()
                        }

                        ToolTip {
                            text: i18nc("@info:tooltip", "Automatically skips chapters containing certain words/characters.\nCheck “Playback” settings for more details.")
                        }
                    }
                }

                ListView {
                    id: listView

                    model: root.m_mpv.chaptersModel
                    reuseItems: true
                    delegate: RadioDelegate {
                        id: menuitem

                        required property int index
                        required property string title
                        required property double startTime

                        property int scrollBarWidth: listViewPage.contentItem.ScrollBar.vertical.width

                        text: `${MiscUtils.formatTime(menuitem.startTime)} - ${menuitem.title}`
                        checked: menuitem.index === root.m_mpv.chapter
                        width: listViewPage.width - scrollBarWidth
                        onClicked: {
                            chaptersPopup.close()
                            root.m_mpv.chapter = menuitem.index
                        }
                        onImplicitWidthChanged: {
                            let popup = chaptersPopup
                            popup.itemBiggestWidth = Math.max(implicitWidth + scrollBarWidth, popup.width)
                            popup.itemHeight = height
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
            function onPositionChanged() {
                if (!slider.seekStarted) {
                    slider.value = root.m_mpv.position
                }
            }
        }
    }
}
