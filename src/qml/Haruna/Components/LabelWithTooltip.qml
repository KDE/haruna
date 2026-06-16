/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls as QQC2

QQC2.Label {
    id: root

    property string toolTipText
    property int toolTipFontSize
    property bool alwaysShowToolTip

    QQC2.ToolTip {
        id: toolTip

        visible: hh.hovered && (root.alwaysShowToolTip || root.truncated)
        text: root.toolTipText ? root.toolTipText : root.text
        font.pointSize: root.toolTipFontSize ? root.toolTipFontSize : root.font.pointSize
    }

    HoverHandler {
        id: hh
    }
}
