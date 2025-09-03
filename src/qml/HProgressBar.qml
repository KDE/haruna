/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQml 2.13
import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Shapes 1.13
import org.kde.kirigami 2.11 as Kirigami

Slider {
    id: root

    required property MpvVideo mpv
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
            width: root.visualPosition * parent.width
            height: parent.height
            color: Kirigami.Theme.highlightColor
            radius: 0
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

            onClicked: function(mouse) {
                if (mouse.button === Qt.MiddleButton) {
                    const time = mouseX * 100 / progressBarBackground.width * root.to / 100
                    const chapters = root.mpv.getProperty("chapter-list")
                    const nextChapter = chapters.findIndex(chapter => chapter.time > time)
                    root.mpv.setProperty("chapter", nextChapter)
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
                progressBarToolTip.text = root.mpv.formatTime(time)
            }

            onEntered: {
                progressBarToolTip.x = mouseX - (progressBarToolTip.width * 0.5)
                progressBarToolTip.y = root.height
            }
        }
    }

    // create markers for the chapters
    Instantiator {
        id: chaptersInstantiator
        model: root.chapters
        delegate: Shape {
            id: chapterMarkerShape

            required property int index
            required property var modelData

            // modelData.time * 100 / mpv.duration is chapter-time percentage
            //  multiplied with progressBarBackground.width / 100 is the percentage at which
            // the chapter marker shoud be positioned on the progress bar
            property int position: modelData.time * 100 / root.mpv.duration * progressBarBackground.width / 100

            antialiasing: true
            parent: progressBarBackground
            ShapePath {
                strokeWidth: 1
                strokeColor: Kirigami.Theme.textColor
                startX: chapterMarkerShape.position
                startY: root.height
                fillColor: Kirigami.Theme.textColor
                PathLine { x: chapterMarkerShape.position; y: -1 }
                PathLine { x: chapterMarkerShape.position + 6; y: -7 }
                PathLine { x: chapterMarkerShape.position - 7; y: -7 }
                PathLine { x: chapterMarkerShape.position - 1; y: -1 }
            }
            Rectangle {
                x: chapterMarkerShape.position - 8
                y: -11
                width: 15
                height: 11
                color: "transparent"
                ToolTip {
                    id: chapterTitleToolTip
                    text: chapterMarkerShape.modelData.title
                    visible: false
                    delay: 0
                    timeout: 10000
                }
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: chapterTitleToolTip.visible = true
                    onExited: chapterTitleToolTip.visible = false
                    onClicked: root.mpv.setProperty("chapter", chapterMarkerShape.index)
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
            checked: settings.get("Playback", "SkipChapters")
            onCheckedChanged: {
                settings.set("Playback", "SkipChapters", checked)
                hSettings.skipChaptersChanged(checked)
            }

            Connections {
                target: hSettings
                onSkipChaptersChanged: skipChaptersMenuItem.checked = checked
            }
        }

        Instantiator {
            model: root.chapters
            delegate: MenuItem {
                id: menuitem

                required property int index
                required property var modelData

                checkable: true
                checked: index === chaptersMenu.checkedItem
                text: `${root.mpv.formatTime(modelData.time)} - ${modelData.title}`
                Component.onCompleted: {
                    chaptersMenu.width = menuitem.width > chaptersMenu.width
                            ? menuitem.width
                            : chaptersMenu.width
                    chaptersMenu.menuItemHeight = height
                }
                onClicked: {
                    root.mpv.setProperty("chapter", index)
                }
            }
            onObjectAdded: chaptersMenu.insertItem(index, object)
            onObjectRemoved: chaptersMenu.removeItem(object)
        }
    }

    Connections {
        target: root.mpv
        onFileLoaded: root.chapters = root.mpv.getProperty("chapter-list")
        onChapterChanged: {
            chaptersMenu.checkedItem = root.mpv.chapter
        }
        onPositionChanged: {
            if (!root.seekStarted) {
                root.value = root.mpv.position
            }
        }
    }
}
