/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import org.kde.kirigami 2.20 as Kirigami

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
