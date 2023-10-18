/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15

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
