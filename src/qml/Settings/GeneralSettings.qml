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
    docPage: "help:/haruna/GeneralSettings.html"

    GridLayout {
        id: content

        columns: 2

        // OSD Font Size
        Label {
            text: i18nc("@label:spinbox", "Osd font size")
            Layout.alignment: Qt.AlignRight
        }

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

        Label {
            text: i18nc("@label:textbox", "File dialog location")
            Layout.alignment: Qt.AlignRight
        }

        TextField {
            id: fileDialogLocation

            text: GeneralSettings.fileDialogLocation
            Layout.fillWidth: true
            onEditingFinished: save()

            Connections {
                target: root
                onSave: fileDialogLocation.save()
            }

            ToolTip {
                text: i18nc("@info:tooltip", "If empty the file dialog will remember the last opened location.")
            }

            function save() {
                GeneralSettings.fileDialogLocation = fileDialogLocation.text
                GeneralSettings.save()
            }
        }

        Label {
            text: i18nc("@label:spinbox", "Maximum recent files")
            Layout.alignment: Qt.AlignRight
        }

        SpinBox {
            id: maxRecentFiles

            from: 0
            to: 100
            value: GeneralSettings.maxRecentFiles
            onValueChanged: {
                GeneralSettings.maxRecentFiles = maxRecentFiles.value
                GeneralSettings.save()
                recentFilesModel.populate()
            }

            ToolTip {
                text: i18nc("@info:tooltip", "How many recent files to store. Enter 0 (zero) to disable.")
            }
        }

        Item { width: 1 }

        CheckBox {
            text: i18nc("@option:check", "Remember window size and position")
            checked: GeneralSettings.rememberWindowGeometry
            onCheckedChanged: {
                GeneralSettings.rememberWindowGeometry = checked
                GeneralSettings.save()
            }
        }

        SettingsHeader {
            text: i18nc("@title", "Interface")
            Layout.columnSpan: 2
            Layout.fillWidth: true
        }

        Label {
            text: i18nc("@label:spinbox", "Preview thumbnail")
            Layout.alignment: Qt.AlignRight
        }

        CheckBox {
            text: i18nc("@option:check", "Show preview thumbnail")
            checked: GeneralSettings.showPreviewThumbnail
            onCheckedChanged: {
                GeneralSettings.showPreviewThumbnail = checked
                GeneralSettings.save()
            }
        }

        Item { width: 1 }

        RowLayout {
        CheckBox {
            text: i18nc("@option:check", "Use accurate preview")
            checked: GeneralSettings.accuratePreviewThumbnail
            enabled: GeneralSettings.showPreviewThumbnail
            onCheckedChanged: {
                GeneralSettings.accuratePreviewThumbnail = checked
                GeneralSettings.save()
            }
        }
        ToolButton {
            icon.name: "documentinfo"
            icon.color: Kirigami.Theme.negativeTextColor
            checkable: true
            checked: false

            ToolTip {
                text: i18nc("@info:tooltip", "Generating an accurate preview is slow")
                visible: parent.checked
                delay: 0
                timeout: -1
                closePolicy: Popup.NoAutoClose
            }
        }
        }

        Item { width: 1 }

        SpinBox {
            id: previewThumbnailWidth

            from: 100
            to: 10000
            value: GeneralSettings.previewThumbnailWidth
            enabled: GeneralSettings.showPreviewThumbnail
            onValueChanged: {
                GeneralSettings.previewThumbnailWidth = previewThumbnailWidth.value
                GeneralSettings.save()
            }
        }

        Item { width: 1 }

        CheckBox {
            text: i18nc("@option:check", "Show menubar")
            checked: GeneralSettings.showMenuBar
            onCheckedChanged: {
                GeneralSettings.showMenuBar = checked
                GeneralSettings.save()
            }
        }

        Item { width: 1 }

        CheckBox {
            text: i18nc("@option:check", "Show toolbar")
            checked: GeneralSettings.showHeader
            onCheckedChanged: {
                GeneralSettings.showHeader = checked
                GeneralSettings.save()
            }
        }

        Item { width: 1 }

        CheckBox {
            text: i18nc("@option:check", "Show chapter markers")
            checked: GeneralSettings.showChapterMarkers
            onCheckedChanged: {
                GeneralSettings.showChapterMarkers = checked
                GeneralSettings.save()
            }
        }

        Label {
            text: i18nc("@label:listbox", "Color scheme")
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

            Component.onCompleted: {
                currentIndex = find(GeneralSettings.colorScheme)
                if (currentIndex === -1) {
                    currentIndex = find("Default")
                }
            }
        }

        Label {
            text: i18nc("@label:listbox", "GUI style")
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
            text: i18nc("@option:check", "Use Breeze icon theme")
            checked: GeneralSettings.useBreezeIconTheme
            onCheckedChanged: {
                GeneralSettings.useBreezeIconTheme = checked
                GeneralSettings.save()
            }
            Layout.row: 14
            Layout.column: 1

            ToolTip {
                text: i18nc("@info:tooltip", "Sets the icon theme to breeze.\nRequires restart.")
            }
        }

        Item {
            width: Kirigami.Units.gridUnit
            height: Kirigami.Units.gridUnit
        }
    }
}
