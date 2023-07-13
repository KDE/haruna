/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQml 2.12
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Shapes 1.12

import org.kde.kirigami 2.11 as Kirigami
import org.kde.haruna 1.0

Slider {
    id: root

    property alias loopIndicator: loopIndicator
    property var chapters: []
    property bool seekStarted: false

    from: 0
    to: mpv.duration
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
            width: endPosition === -1 ? 1 : (endPosition / mpv.duration * progressBarBG.width) - x
            height: parent.height
            color: Qt.hsla(0, 0, 0, 0.4)
            visible: startPosition !== -1
            x: startPosition / mpv.duration * progressBarBG.width
            z: 110
        }

        Rectangle {
            width: root.position * parent.width
            height: parent.height
            color: Kirigami.Theme.highlightColor
        }

        ToolTip {
            id: progressBarToolTip

            visible: progressBarMouseArea.containsMouse && mpv.currentFile !== ""
            timeout: -1
            delay: 0
            contentItem: ColumnLayout {

                MpvPreview {
                    id: previewMpv

                    property double aspectRatio: 0

                    visible: previewMpv.file !== ""
                             && previewMpv.isLocalFile
                             && GeneralSettings.showPreviewThumbnail
                    accuratePreview: GeneralSettings.accuratePreviewThumbnail

                    Layout.preferredWidth: GeneralSettings.previewThumbnailWidth
                    Layout.preferredHeight: Math.ceil(Layout.preferredWidth / aspectRatio)
                    Layout.alignment: Qt.AlignCenter

                    Connections {
                        target: mpv
                        onVideoReconfig: {
                            let width = mpv.getProperty("width");
                            let height = mpv.getProperty("height");
                            let ar = width / height;
                            previewMpv.aspectRatio = ar
                        }
                        onFileLoaded: {
                            previewMpv.file = mpv.currentFile
                        }
                    }
                }

                Label {
                    Layout.alignment: Qt.AlignCenter
                    text: progressBarToolTip.text
                }
            }

        }

        MouseArea {
            id: progressBarMouseArea

            anchors.fill: parent
            hoverEnabled: true
            acceptedButtons: Qt.MiddleButton | Qt.RightButton

            onClicked: {
                if (mouse.button === Qt.MiddleButton) {
                    if (!GeneralSettings.showChapterMarkers) {
                        return
                    }

                    const time = mouseX * 100 / progressBarBG.width * root.to / 100
                    const chapters = mpv.getProperty("chapter-list")
                    const nextChapter = chapters.findIndex(chapter => chapter.time > time)
                    mpv.chapter = nextChapter
                }
                if (mouse.button === Qt.RightButton && root.chapters.length > 0) {
                    chaptersPopup.x = mouseX - chaptersPopup.width * 0.5
                    chaptersPopup.open()
                }
            }

            onMouseXChanged: {
                progressBarToolTip.x = mouseX - (progressBarToolTip.width * 0.5)

                const time = mouseX * 100 / progressBarBG.width * root.to / 100
                previewMpv.position = time
                progressBarToolTip.text = app.formatTime(time)
            }

            onEntered: {
                progressBarToolTip.x = mouseX - (progressBarToolTip.width * 0.5)
                progressBarToolTip.y = root.height + Kirigami.Units.largeSpacing
            }

            onWheel: {
                if (wheel.angleDelta.y > 0) {
                    appActions.seekForwardMediumAction.trigger()
                } else if (wheel.angleDelta.y) {
                    appActions.seekBackwardMediumAction.trigger()
                }
            }
        }
    }

    // create markers for the chapters
    Repeater {
        id: chaptersInstantiator
        model: GeneralSettings.showChapterMarkers ? chapters : 0
        delegate: Shape {
            id: chapterMarkerShape

            // where the chapter marker shoud be positioned on the progress bar
            property int position: root.mirrored
                                   ? progressBarBG.width - (modelData.time / mpv.duration * progressBarBG.width)
                                   : modelData.time / mpv.duration * progressBarBG.width

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
                    text: modelData.title
                    visible: false
                    delay: 0
                    timeout: 10000
                }
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: chapterTitleToolTip.visible = true
                    onExited: chapterTitleToolTip.visible = false
                    onClicked: mpv.setProperty("chapter", index)
                }
            }
        }
    }

    onToChanged: value = mpv.position
    onPressedChanged: {
        if (pressed) {
            seekStarted = true
        } else {
            mpv.command(["seek", value, "absolute"])
            seekStarted = false
        }
    }

    Popup {
        id: chaptersPopup

        property int itemHeight
        property int itemBiggestWidth
        property var checkedItem
        property int maxWidth: window.width * 0.7 > Kirigami.Units.gridUnit * 40
                               ? Kirigami.Units.gridUnit * 40
                               : window.width * 0.7

        y: -height - root.height
        width: itemBiggestWidth > maxWidth ? maxWidth : itemBiggestWidth
        height: itemHeight * root.chapters.length + listViewPage.footer.height > mpv.height - Kirigami.Units.gridUnit
                ? mpv.height - Kirigami.Units.gridUnit
                : itemHeight * root.chapters.length + listViewPage.footer.height
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

                model: root.chapters
                delegate: CheckDelegate {
                    id: menuitem

                    text: `${app.formatTime(modelData.time)} - ${modelData.title}`
                    checked: index === chaptersPopup.checkedItem
                    width: listViewPage.width
                    onClicked: {
                        chaptersPopup.close()
                        mpv.chapter = index
                    }
                    Component.onCompleted: {
                        const scrollBarWidth = listViewPage.contentItem.ScrollBar.vertical.width
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
        target: mpv
        onFileLoaded: root.chapters = mpv.getProperty("chapter-list")
        onChapterChanged: {
            chaptersPopup.checkedItem = mpv.chapter
        }
        onPositionChanged: {
            if (!root.seekStarted) {
                root.value = mpv.position
            }
        }
    }
}
