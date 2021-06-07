/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import org.kde.kirigami 2.11 as Kirigami
import org.kde.haruna 1.0

import Haruna.Components 1.0

SettingsBasePage {
    id: root

    hasHelp: true
    helpFile: ":/GeneralSettings.html"

    GridLayout {
        id: content

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

                // used to prevent osd showing when opening the page
                property bool completed: false

                editable: true
                from: 0
                to: 100
                value: GeneralSettings.osdFontSize
                onValueChanged: {
                    if (completed) {
                        osd.label.font.pointSize = osdFontSize.value
                        osd.message("Test osd font size")
                        GeneralSettings.osdFontSize = osdFontSize.value
                        GeneralSettings.save()
                    }
                }
                Component.onCompleted: completed = true
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
                value: GeneralSettings.volumeStep
                onValueChanged: {
                    if (root.visible) {
                        GeneralSettings.volumeStep = volumeStep.value
                        GeneralSettings.save()
                    }
                }
            }
            Layout.fillWidth: true
        }

        // Seek Small Step
        Label {
            text: qsTr("Seek small step")
            Layout.alignment: Qt.AlignRight
        }

        Item {
            height: seekSmallStep.height
            SpinBox {
                id: seekSmallStep
                editable: true
                from: 0
                to: 100
                value: GeneralSettings.seekSmallStep
                onValueChanged: {
                    GeneralSettings.seekSmallStep = seekSmallStep.value
                    GeneralSettings.save()
                }
            }
            Layout.fillWidth: true
        }

        // Seek Medium Step
        Label {
            text: qsTr("Seek medium step")
            Layout.alignment: Qt.AlignRight
        }

        Item {
            height: seekMediumStep.height
            SpinBox {
                id: seekMediumStep
                editable: true
                from: 0
                to: 100
                value: GeneralSettings.seekMediumStep
                onValueChanged: {
                    GeneralSettings.seekMediumStep = seekMediumStep.value
                    GeneralSettings.save()
                }
            }
            Layout.fillWidth: true
        }

        // Seek Big Step
        Label {
            text: qsTr("Seek big step")
            Layout.alignment: Qt.AlignRight
        }

        Item {
            height: seekBigStep.height
            SpinBox {
                id: seekBigStep
                editable: true
                from: 0
                to: 100
                value: GeneralSettings.seekBigStep
                onValueChanged: {
                    GeneralSettings.seekBigStep = seekBigStep.value
                    GeneralSettings.save()
                }
            }
            Layout.fillWidth: true
        }

        Label {
            text: qsTr("File dialog location")
            Layout.alignment: Qt.AlignRight
        }

        Item {
            height: fileDialogLocation.height
            Layout.fillWidth: true

            TextField {
                id: fileDialogLocation

                text: GeneralSettings.fileDialogLocation
                onEditingFinished: {
                    GeneralSettings.fileDialogLocation = fileDialogLocation.text
                    GeneralSettings.save()
                }

                ToolTip {
                    text: qsTr("If empty the file dialog will remember the last opened location.")
                }
            }
        }

        SettingsHeader {
            text: qsTr("Interface")
            Layout.columnSpan: 2
            Layout.fillWidth: true
        }

        CheckBox {
            text: qsTr("Show MenuBar")
            checked: GeneralSettings.showMenuBar
            onCheckedChanged: {
                GeneralSettings.showMenuBar = checked
                GeneralSettings.save()
            }
            Layout.row: 8
            Layout.column: 1
        }

        CheckBox {
            text: qsTr("Show Header")
            checked: GeneralSettings.showHeader
            onCheckedChanged: {
                GeneralSettings.showHeader = checked
                GeneralSettings.save()
            }
            Layout.row: 9
            Layout.column: 1
        }

        CheckBox {
            text: qsTr("Show chapter markers")
            checked: GeneralSettings.showChapterMarkers
            onCheckedChanged: {
                GeneralSettings.showChapterMarkers = checked
                GeneralSettings.save()
            }
            Layout.row: 10
            Layout.column: 1
        }

        Label {
            text: qsTr("Color scheme")
            Layout.alignment: Qt.AlignRight
        }

        ComboBox {
            id: colorThemeSwitcher

            textRole: "display"
            model: app.colorSchemesModel
            delegate: ItemDelegate {
                Kirigami.Theme.colorSet: Kirigami.Theme.View
                width: colorThemeSwitcher.width
                highlighted: model.display === GeneralSettings.colorScheme
                contentItem: RowLayout {
                    Kirigami.Icon {
                        source: model.decoration
                        Layout.preferredHeight: Kirigami.Units.iconSizes.small
                        Layout.preferredWidth: Kirigami.Units.iconSizes.small
                    }
                    Label {
                        text: model.display
                        color: highlighted ? Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor
                        Layout.fillWidth: true
                    }
                }
            }

            onActivated: {
                GeneralSettings.colorScheme = colorThemeSwitcher.textAt(index)
                GeneralSettings.save()
                app.activateColorScheme(GeneralSettings.colorScheme)
            }

            Component.onCompleted: currentIndex = find(GeneralSettings.colorScheme)
        }

        Label {
            text: qsTr("GUI Style")
            Layout.alignment: Qt.AlignRight
        }

        ComboBox {
            id: guiStyleComboBox

            textRole: "key"
            model: ListModel {
                id: stylesModel

                ListElement { key: "Default"; }
            }

            onActivated: {
                GeneralSettings.guiStyle = model.get(index).key
                app.setGuiStyle(GeneralSettings.guiStyle)
                // some themes can cause a crash
                // the timer prevents saving the crashing theme,
                // which would cause the app to crash on startup
                saveGuiStyleTimer.start()
            }

            Timer {
                id: saveGuiStyleTimer

                interval: 1000
                running: false
                repeat: false
                onTriggered: GeneralSettings.save()
            }

            Component.onCompleted: {
                // populate the model with the available styles
                for (let i = 0; i < app.availableGuiStyles().length; ++i) {
                    stylesModel.append({key: app.availableGuiStyles()[i]})
                }

                // set the saved style as the current item in the combo box
                for (let j = 0; j < stylesModel.count; ++j) {
                    if (stylesModel.get(j).key === GeneralSettings.guiStyle) {
                        currentIndex = j
                        break
                    }
                }
            }
        }

        CheckBox {
            text: qsTr("Use Breeze icon theme")
            checked: GeneralSettings.useBreezeIconTheme
            onCheckedChanged: {
                GeneralSettings.useBreezeIconTheme = checked
                GeneralSettings.save()
            }
            Layout.row: 14
            Layout.column: 1

            ToolTip {
                text: qsTr("Sets the icon theme to breeze.\nRequires restart.")
            }
        }

        Item {
            width: Kirigami.Units.gridUnit
            height: Kirigami.Units.gridUnit
        }
    }
}
