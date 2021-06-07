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
    property var chapters
    property bool seekStarted: false

    from: 0
    to: mpv.duration
    implicitWidth: 200
    implicitHeight: 25
    leftPadding: 0
    rightPadding: 0

    handle: Item { visible: false }

    background: Rectangle {
        id: progressBarBackground
        color: Kirigami.Theme.alternateBackgroundColor

        Rectangle {
            id: loopIndicator
            property double startPosition: -1
            property double endPosition: -1
            width: endPosition === -1 ? 1 : (endPosition / mpv.duration * progressBarBackground.width) - x
            height: parent.height
            color: Qt.hsla(0, 0, 0, 0.4)
            visible: startPosition !== -1
            x: startPosition / mpv.duration * progressBarBackground.width
            z: 110
        }

        Rectangle {
            width: visualPosition * parent.width
            height: parent.height
            color: Kirigami.Theme.highlightColor
        }

        ToolTip {
            id: progressBarToolTip

            visible: progressBarMouseArea.containsMouse
            timeout: -1
            delay: 0
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

                    const time = mouseX * 100 / progressBarBackground.width * root.to / 100
                    const chapters = mpv.getProperty("chapter-list")
                    const nextChapter = chapters.findIndex(chapter => chapter.time > time)
                    mpv.chapter = nextChapter
                }
                if (mouse.button === Qt.RightButton && root.chapters.length > 0) {
                    const menuX = mouse.x-chaptersMenu.width * 0.5
                    const menuY = -((chaptersMenu.count - 1) * chaptersMenu.menuItemHeight + 15)
                    chaptersMenu.popup(root, menuX, menuY)
                }
            }

            onMouseXChanged: {
                progressBarToolTip.x = mouseX - (progressBarToolTip.width * 0.5)

                const time = mouseX * 100 / progressBarBackground.width * root.to / 100
                progressBarToolTip.text = app.formatTime(time)
            }

            onEntered: {
                progressBarToolTip.x = mouseX - (progressBarToolTip.width * 0.5)
                progressBarToolTip.y = root.height
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
            property int position: modelData.time / mpv.duration * progressBarBackground.width

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

    Menu {
        id: chaptersMenu

        property int menuItemHeight
        property var checkedItem

        width: 0
        modal: true

        MenuSeparator {}

        MenuItem {
            id: skipChaptersMenuItem
            text: qsTr("Skip Chapters")
            checkable: true
            checked: PlaybackSettings.skipChapters
            onCheckedChanged: {
                PlaybackSettings.skipChapters = checked
                PlaybackSettings.save()
            }
        }

        Instantiator {
            model: root.chapters
            delegate: MenuItem {
                id: menuitem

                checkable: true
                checked: index === chaptersMenu.checkedItem
                text: `${app.formatTime(modelData.time)} - ${modelData.title}`
                Component.onCompleted: {
                    chaptersMenu.width = menuitem.width > chaptersMenu.width
                            ? menuitem.width
                            : chaptersMenu.width
                    chaptersMenu.menuItemHeight = height
                }
                onClicked: mpv.chapter = index
            }
            onObjectAdded: chaptersMenu.insertItem(index, object)
            onObjectRemoved: chaptersMenu.removeItem(object)
        }
    }

    Connections {
        target: mpv
        onFileLoaded: chapters = mpv.getProperty("chapter-list")
        onChapterChanged: {
            chaptersMenu.checkedItem = mpv.chapter
        }
        onPositionChanged: {
            if (!root.seekStarted) {
                root.value = mpv.position
            }
        }
    }
}
