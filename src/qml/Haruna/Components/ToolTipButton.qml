/*
 * SPDX-FileCopyrightText: 2024 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls

ToolButton {
    id: root

    required property string toolTipText
    property alias toolTipWidth: toolTip.width
    property alias toolTipHeight: toolTip.height
    readonly property int maxWidth: 300

    icon.name: "documentinfo"
    checkable: true
    checked: false

    ToolTip {
        id: toolTip

        y: root.height
        visible: root.checked
        padding: 0
        implicitWidth: Math.min(root.maxWidth, textArea.contentWidth)
        implicitHeight: textArea.implicitHeight

        delay: 0
        timeout: -1
        closePolicy: Popup.NoAutoClose

        contentItem: ScrollView {
            TextArea {
                id: textArea

                text: root.toolTipText
                background: Rectangle {
                    color: "transparent"
                }
                readOnly: true
                textFormat: Text.RichText
                wrapMode: Text.WordWrap
                selectByMouse: true
                padding: 15
                onLinkActivated: function(link) {
                    Qt.openUrlExternally(link)
                }
                onHoveredLinkChanged: hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
            }
        }
    }
}
