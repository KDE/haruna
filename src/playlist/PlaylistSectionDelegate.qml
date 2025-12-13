/*
 * SPDX-FileCopyrightText: 2025 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.haruna
import org.kde.haruna.settings

Item {
    id: root

    required property string section
    required property PlaylistFilterProxyModel model

    property real alpha: PlaylistSettings.overlayVideo ? 0.6 : 1

    implicitWidth: ListView.view.width
    implicitHeight: content.implicitHeight + Kirigami.Units.largeSpacing * 3

    Rectangle {
        id: backgroundRect

        anchors {
            fill: parent
            bottomMargin: 1
        }

        color: Qt.alpha(Qt.darker(Kirigami.Theme.backgroundColor, 1.1), root.alpha)

        ColumnLayout {
            id: content

            anchors.fill: parent

            RowLayout {
                spacing: 1
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.leftMargin: Kirigami.Units.largeSpacing * 2
                Layout.rightMargin: Kirigami.Units.mediumSpacing

                ColumnLayout {
                    id: sectionColumn

                    Layout.fillWidth: true

                    Repeater {
                        model: root.model.getSectionList(root.section)

                        delegate: Kirigami.Heading {
                            required property int index
                            required property string modelData

                            text: modelData
                            level: index + 1
                            type: Kirigami.Heading.Type.Primary
                            color: Kirigami.Theme.textColor
                            maximumLineCount: 1
                            wrapMode: Text.Wrap
                            elide: Text.ElideRight
                            width: parent.width
                            Layout.fillWidth: true
                        }
                    }
                }
            }
        }
    }
}
