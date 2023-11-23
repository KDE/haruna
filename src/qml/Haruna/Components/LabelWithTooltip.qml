/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

QQC2.Label {
    id: root

    property string toolTipText
    property int toolTipFontSize
    property bool alwaysShowToolTip

    QQC2.ToolTip {
        id: toolTip

        visible: (root.alwaysShowToolTip && mouseArea.containsMouse) || (mouseArea.containsMouse && root.truncated)
        text: root.toolTipText ? root.toolTipText : root.text
        font.pointSize: root.toolTipFontSize ? root.toolTipFontSize : root.font.pointSize
    }

    MouseArea {
        id: mouseArea

        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        hoverEnabled: true
    }
}
