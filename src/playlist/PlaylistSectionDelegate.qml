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
    implicitHeight: content.implicitHeight

    anchors {
        topMargin: 1
        bottomMargin: 1
    }

    Rectangle {
        id: backgroundRect

        anchors {
            fill: parent
            topMargin: 1
            bottomMargin: 1
        }

        color: Qt.alpha(Kirigami.Theme.backgroundColor, root.alpha)

        ColumnLayout {
            id: content

            anchors.fill: parent

            RowLayout {
                spacing: 1
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.leftMargin: Kirigami.Units.mediumSpacing
                Layout.rightMargin: Kirigami.Units.mediumSpacing
                Layout.topMargin: Kirigami.Units.mediumSpacing

                Rectangle {
                    color: Qt.alpha(Kirigami.Theme.activeTextColor, root.alpha)
                    Layout.preferredWidth: 5
                    Layout.fillWidth: false
                    Layout.fillHeight: true
                }

                Kirigami.Separator {
                    color: Qt.alpha(Kirigami.Theme.activeTextColor, root.alpha)
                    weight: Kirigami.Separator.Weight.Normal
                    Layout.fillHeight: true
                    Layout.fillWidth: false
                }

                Kirigami.Separator {
                    color: Qt.alpha(Kirigami.Theme.activeTextColor, root.alpha)
                    weight: Kirigami.Separator.Weight.Light
                    Layout.fillHeight: true
                    Layout.fillWidth: false
                    Layout.rightMargin: Kirigami.Units.smallSpacing
                }

                ColumnLayout {
                    id: sectionColumn

                    Layout.fillWidth: true

                    Repeater {
                        model: root.model.getSectionList(root.section)
                        Layout.fillWidth: true

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

            Rectangle {
                id: bottomLine

                color: Qt.alpha(Kirigami.Theme.activeTextColor, root.alpha)
                Layout.preferredHeight: 1
                Layout.fillWidth: true
                Layout.fillHeight: false
                Layout.alignment: Qt.AlignBottom
                Layout.leftMargin: Kirigami.Units.mediumSpacing
                Layout.rightMargin: Kirigami.Units.mediumSpacing
                Layout.bottomMargin: Kirigami.Units.mediumSpacing
            }
        }
    }
}
