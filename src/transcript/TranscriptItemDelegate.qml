/*
* SPDX-FileCopyrightText: 2026 George Florea Bănuș <georgefb899@gmail.com>
* SPDX-FileCopyrightText: 2026 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

ItemDelegate {
    id: root

    required property int index
    required property string subtitleText
    required property var splittedText
    required property real duration
    required property real startTime
    required property string formattedStartTime
    required property string formattedEndTime
    required property bool isCurrent

    readonly property bool expand: root.highlighted || root.hovered
    // Baseline for the delegate's height. Every Label component shares the same font size therefore their height is the same.
    readonly property real labelHeight: endTime.implicitHeight
    // Minimum delegate height should contain 2 Labels
    readonly property real labelCount: root.expand ? Math.max(readOnlyText.lineCount, 2) : 2

    signal seek(time: real)

    highlighted: root.isCurrent
    hoverEnabled: true
    implicitWidth: ListView.view.width
    implicitHeight: Math.round(root.labelHeight * root.labelCount) + Kirigami.Units.largeSpacing * 2

    Behavior on implicitHeight {
        NumberAnimation {
            duration: Kirigami.Units.longDuration
            easing.type: Easing.OutCubic
        }
    }

    onClicked: root.seek(root.startTime / 1000.0)

    contentItem: RowLayout {
        spacing: Kirigami.Units.mediumSpacing

        ColumnLayout {
            id: timestamp

            spacing: 0
            Layout.leftMargin: Kirigami.Units.mediumSpacing
            Layout.alignment: Qt.AlignVCenter

            Label {
                text: root.formattedStartTime
                color: root.getLabelColor()
            }

            Label {
                id: endTime

                text: root.formattedEndTime
                color: root.getLabelColor()
            }
        }

        Rectangle {
            color: Qt.alpha(Kirigami.Theme.textColor, 0.2)
            Layout.preferredWidth: 1
            Layout.fillHeight: true
            Layout.topMargin: Kirigami.Units.smallSpacing
            Layout.bottomMargin: Kirigami.Units.smallSpacing
        }

        Item {
            clip: true
            Layout.fillWidth: true
            Layout.fillHeight: true

            // Default state
            Column {
                id: subtitleLineColumn

                anchors {
                    margins: Kirigami.Units.largeSpacing
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    right: parent.right
                }

                spacing: 0

                Label {
                    color: root.getLabelColor()
                    elide: Text.ElideRight
                    height: implicitHeight
                    width: subtitleLineColumn.width
                    horizontalAlignment: Qt.AlignLeft
                    verticalAlignment: Qt.AlignVCenter
                    padding: 0
                    text: root.splittedText[0] ?? ""
                    visible: !root.expand && root.splittedText.length > 0
                    wrapMode: Text.NoWrap
                }

                Label {
                    color: root.getLabelColor()
                    elide: Text.ElideRight
                    height: implicitHeight
                    width: subtitleLineColumn.width
                    horizontalAlignment: Qt.AlignLeft
                    verticalAlignment: Qt.AlignVCenter
                    padding: 0
                    text: root.splittedText[1] ?? ""
                    visible: !root.expand && root.splittedText.length > 1
                    wrapMode: Text.NoWrap
                }
            }

            // Expanded state
            TextEdit {
                id: readOnlyText

                anchors {
                    margins: Kirigami.Units.largeSpacing
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    right: parent.right
                }

                color: root.getLabelColor()
                visible: root.expand
                horizontalAlignment: Qt.AlignLeft
                verticalAlignment: Qt.AlignVCenter
                padding: 0
                readOnly: true
                text: root.subtitleText
                wrapMode: TextEdit.Wrap
                selectByMouse: true
            }
        }
    }

    function getLabelColor() {
        if (root.highlighted) {
            return Kirigami.Theme.highlightedTextColor
        }
        return Kirigami.Theme.textColor
    }
}
