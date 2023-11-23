/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import org.kde.kirigami as Kirigami

ColumnLayout {
    id: root

    property string text: ""
    property int topMargin: Kirigami.Units.gridUnit

    spacing: 0

    Item {
        width: 1
        height: root.topMargin
        visible: root.topMargin > 0
    }

    RowLayout {
        Rectangle {
            width: Kirigami.Units.gridUnit
            height: 1
            color: Kirigami.Theme.alternateBackgroundColor
        }

        Kirigami.Heading {
            text: root.text
        }

        Rectangle {
            height: 1
            color: Kirigami.Theme.alternateBackgroundColor
            Layout.fillWidth: true
        }
    }
}
