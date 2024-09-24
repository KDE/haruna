/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import org.kde.kirigami 2.11 as Kirigami
import AppSettings 1.0

Item {
    id: root

    property bool hasHelp: false
    property string helpFile: ""

    visible: false

    GridLayout {
        id: content

        width: parent.width
        columns: 2

        // OSD Font Size
        Label {
            text: qsTr("Osd font size")
            Layout.alignment: Qt.AlignRight
        }

        Item {
            height: osdFontSize.height
            SpinBox {
                id: osdFontSize
                editable: true
                from: 0
                to: 100
                value: AppSettings.osdFontSize
                onValueChanged: {
                    // runs on start-up so only execute when state is visible
                    if (root.visible) {
                        osd.label.font.pointSize = osdFontSize.value
                        osd.message("Test osd font size")
                        AppSettings.osdFontSize = osdFontSize.value
                    }
                }
            }
            Layout.fillWidth: true
        }

        // Volume Step
        Label {
            text: qsTr("Volume step")
            Layout.alignment: Qt.AlignRight
        }

        Item {
            height: volumeStep.height
            SpinBox {
                id: volumeStep
                editable: true
                from: 0
                to: 100
                value: AppSettings.volumeStep
                onValueChanged: {
                    if (root.visible) {
                        AppSettings.volumeStep = volumeStep.value
                    }
                }
            }
            Layout.fillWidth: true
        }

        Item {
            Layout.columnSpan: 2
            height: 5
            Rectangle {
                y: 2
                width: content.width
                height: 1
                color: Kirigami.Theme.alternateBackgroundColor
            }
        }

        // Seek Small Step
        Label {
            text: qsTr("Seek Small Step")
            Layout.alignment: Qt.AlignRight
        }

        Item {
            height: seekSmallStep.height
            SpinBox {
                id: seekSmallStep
                editable: true
                from: 0
                to: 100
                value: AppSettings.seekSmallStep
                onValueChanged: AppSettings.seekSmallStep = seekSmallStep.value
            }
            Layout.fillWidth: true
        }

        // Seek Medium Step
        Label {
            text: qsTr("Seek Medium Step")
            Layout.alignment: Qt.AlignRight
        }

        Item {
            height: seekMediumStep.height
            SpinBox {
                id: seekMediumStep
                editable: true
                from: 0
                to: 100
                value: AppSettings.seekMediumStep
                onValueChanged: AppSettings.seekMediumStep = seekMediumStep.value
            }
            Layout.fillWidth: true
        }

        // Seek Big Step
        Label {
            text: qsTr("Seek Big Step")
            Layout.alignment: Qt.AlignRight
        }

        Item {
            height: seekBigStep.height
            SpinBox {
                id: seekBigStep
                editable: true
                from: 0
                to: 100
                value: AppSettings.seekBigStep
                onValueChanged: AppSettings.seekBigStep = seekBigStep.value
            }
            Layout.fillWidth: true
        }

        Item {
            Layout.columnSpan: 2
            Layout.fillHeight: true
            Layout.fillWidth: true
        }

    }

    Connections {
        target: settingsEditor
        function onVisibleChanged() {
            visible = settingsEditor.visible
        }
    }
}
