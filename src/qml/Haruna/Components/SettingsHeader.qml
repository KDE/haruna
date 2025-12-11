/*
 * SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

ColumnLayout {
    id: root

    property int headingLevel: 1
    property string text: ""
    property int topMargin: Kirigami.Units.gridUnit

    spacing: 0

    RowLayout {
        Layout.topMargin: root.topMargin

        Rectangle {
            color: Kirigami.Theme.alternateBackgroundColor
            Layout.preferredWidth: Kirigami.Units.gridUnit
            Layout.preferredHeight: 1
        }

        Kirigami.Heading {
            text: root.text
            level: root.headingLevel
        }

        Rectangle {
            color: Kirigami.Theme.alternateBackgroundColor
            Layout.preferredHeight: 1
            Layout.fillWidth: true
        }
    }
}
