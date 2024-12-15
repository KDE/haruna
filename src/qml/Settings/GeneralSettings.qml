/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import org.kde.kirigami as Kirigami
import org.kde.haruna
import org.kde.haruna.settings

SettingsBasePage {
    id: root

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
                function onSave() {
                    fileDialogLocation.save()
                }
            }

            ToolTip {
                text: i18nc("@info:tooltip", "The default location of the open file dialog. If empty " +
                            "the file dialog will try to open to the last opened location.")
            }

            function save() : void {
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

        Item { Layout.preferredWidth: 1 }

        RowLayout {
            CheckBox {
                text: i18nc("@option:check", "Allow only one instance")
                checked: GeneralSettings.useSingleInstance
                onCheckedChanged: {
                    GeneralSettings.useSingleInstance = checked
                    GeneralSettings.save()
                }
            }

            ToolButton {
                icon.name: "documentinfo"
                checkable: true
                checked: false
                Layout.preferredHeight: Kirigami.Units.iconSizes.medium

                ToolTip {
                    text: i18nc("@info:tooltip",
                                "Trying to open another Haruna instance will do nothing.\n"+
                                "Trying to open another file with Haruna will open the file in the running instance.")
                    visible: (parent as ToolButton).checked
                    delay: 0
                    timeout: -1
                    closePolicy: Popup.NoAutoClose
                }
            }
        }

        Item { Layout.preferredWidth: 1 }

        RowLayout {
            CheckBox {
                text: i18nc("@option:check", "Add file to playlist")
                checked: GeneralSettings.appendVideoToSingleInstance
                enabled: GeneralSettings.useSingleInstance
                onCheckedChanged: {
                    GeneralSettings.appendVideoToSingleInstance = checked
                    GeneralSettings.save()
                }
            }

            ToolButton {
                icon.name: "documentinfo"
                checkable: true
                checked: false
                Layout.preferredHeight: Kirigami.Units.iconSizes.medium

                ToolTip {
                    text: i18nc("@info:tooltip", "File will be added to the end of the playlist")
                    visible: (parent as ToolButton).checked
                    delay: 0
                    timeout: -1
                    closePolicy: Popup.NoAutoClose
                }
            }
        }
        SettingsHeader {
            text: i18nc("@title", "Interface")
            Layout.columnSpan: 2
            Layout.fillWidth: true
        }

        Label {
            text: i18nc("@label:spinbox", "Footer style")
            Layout.alignment: Qt.AlignRight
        }

        ComboBox {
            textRole: "display"
            model: ListModel {
                ListElement { display: "Default"; value: "default" }
                ListElement { display: "Floating"; value: "floating" }
            }
            Component.onCompleted: {
                for (let i = 0; i < model.count; ++i) {
                    if (model.get(i).value === GeneralSettings.footerStyle) {
                        currentIndex = i
                        break
                    }
                }
            }
            onActivated: function(index) {
                GeneralSettings.footerStyle = model.get(index).value
                GeneralSettings.save()
            }
        }

        Label {
            text: i18nc("@label:spinbox", "Show floating footer on")
            Layout.alignment: Qt.AlignRight
        }

        RowLayout {
            ComboBox {
                textRole: "display"
                enabled: GeneralSettings.footerStyle === "floating"
                model: ListModel {
                    ListElement { display: "Every mouse movement"; value: "EveryMouseMovement" }
                    ListElement { display: "Bottom mouse movement"; value: "BottomMouseMovement" }
                }

                Component.onCompleted: {
                    for (let i = 0; i < model.count; ++i) {
                        if (model.get(i).value === GeneralSettings.floatingFooterTrigger) {
                            currentIndex = i
                            break
                        }
                    }
                }
                onActivated: function(index) {
                    GeneralSettings.floatingFooterTrigger = model.get(index).value
                    GeneralSettings.save()
                }
            }

            ToolTipButton {
                toolTipText: i18nc("@info:tooltip",
                                   "What action shows the footer.<br>" +
                                   "<strong>Every mouse movement</strong> - every mouse movement over the video area<br>" +
                                   "<strong>Bottom mouse movement</strong> - mouse movement on the bottom of the video area")
            }

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

            ToolTip.text: i18nc("@info:tooltip", "Shows a preview while hovering the seek/progress bar.")
            ToolTip.visible: hovered
            ToolTip.delay: 700
        }

        Item { Layout.preferredWidth: 1 }

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
                Layout.preferredHeight: Kirigami.Units.iconSizes.medium

                ToolTip {
                    text: i18nc("@info:tooltip", "Generating an accurate preview is slow")
                    visible: (parent as ToolButton).checked
                    delay: 0
                    timeout: -1
                    closePolicy: Popup.NoAutoClose
                }
            }
        }

        Label {
            text: i18nc("@label:spinbox", "Thumbnail width")
            Layout.alignment: Qt.AlignRight
        }

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

        Label {
            text: i18nc("@label", "Window")
            Layout.alignment: Qt.AlignRight
        }

        RowLayout {
            CheckBox {
                text: i18nc("@option:check", "Resize to fit video")
                checked: GeneralSettings.resizeWindowToVideo
                enabled: !HarunaApp.isPlatformWayland()
                onCheckedChanged: {
                    GeneralSettings.resizeWindowToVideo = checked
                    GeneralSettings.save()
                    window.resizeWindow()
                }
            }

            ToolButton {
                icon.name: "documentinfo"
                checkable: true
                checked: false
                Layout.preferredHeight: Kirigami.Units.iconSizes.medium

                ToolTip {
                    text: HarunaApp.isPlatformWayland()
                          ? i18nc("@info:tooltip","Not supported on Wayland.")
                          : i18nc("@info:tooltip", "The window is resized according to the video resolution.\n" +
                                  "The maximum size is not constrained, this is left to the operating system.")
                    visible: (parent as ToolButton).checked
                    delay: 0
                    timeout: -1
                    closePolicy: Popup.NoAutoClose
                }
            }
        }

        Item { Layout.preferredWidth: 1 }
        RowLayout {
            CheckBox {
                text: i18nc("@option:check", "Start in fullscreen mode")
                checked: GeneralSettings.fullscreenOnStartUp
                onCheckedChanged: {
                    GeneralSettings.fullscreenOnStartUp = checked
                    GeneralSettings.save()
                }
            }

            ToolButton {
                icon.name: "documentinfo"
                checkable: true
                checked: false
                Layout.preferredHeight: Kirigami.Units.iconSizes.medium

                ToolTip {
                    text: i18nc("@info:tooltip", "Enter fullscreen mode when the application starts.\n" +
                                "This takes precedence over Resize to fit video, and Remember window size and position settings.")
                    visible: (parent as ToolButton).checked
                    delay: 0
                    timeout: -1
                    closePolicy: Popup.NoAutoClose
                }
            }
        }

        Item { Layout.preferredWidth: 1 }

        RowLayout {
            CheckBox {
                text: i18nc("@option:check", "Remember window size and position")
                checked: GeneralSettings.rememberWindowGeometry
                enabled: !GeneralSettings.resizeWindowToVideo
                onCheckedChanged: {
                    GeneralSettings.rememberWindowGeometry = checked
                    GeneralSettings.save()
                }
            }

            ToolButton {
                icon.name: "documentinfo"
                icon.color: HarunaApp.isPlatformWayland() ? Kirigami.Theme.negativeTextColor : Kirigami.Theme.textColor
                checkable: true
                checked: false
                Layout.preferredHeight: Kirigami.Units.iconSizes.medium

                ToolTip {
                    readonly property
                    string waylandMessage: HarunaApp.isPlatformWayland()
                                           ? i18nc("@info:tooltip extra wayland info for the \"Remember window size and position\" setting",
                                                   "<b>Restoring position is not supported on Wayland.</b><br><br>")
                                           : ""
                    text: i18nc("@info:tooltip", "Changes to the window's size and position "
                                +"are saved and used for newly opened windows.<br><br>"
                                +"%1The \"Resize to fit video\" setting takes precedence.", waylandMessage)
                    visible: (parent as ToolButton).checked
                    delay: 0
                    timeout: -1
                    closePolicy: Popup.NoAutoClose
                }
            }
        }

        Item { Layout.preferredWidth: 1 }

        CheckBox {
            text: i18nc("@option:check", "Show menubar")
            checked: GeneralSettings.showMenuBar
            onCheckedChanged: {
                GeneralSettings.showMenuBar = checked
                GeneralSettings.save()
            }
        }

        Item { Layout.preferredWidth: 1 }

        CheckBox {
            text: i18nc("@option:check", "Show toolbar")
            checked: GeneralSettings.showHeader
            onCheckedChanged: {
                GeneralSettings.showHeader = checked
                GeneralSettings.save()
            }
        }

        Item { Layout.preferredWidth: 1 }

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
            model: HarunaApp.colorSchemesModel
            delegate: ItemDelegate {
                id: delegate

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
                        color: delegate.highlighted ? Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor
                        Layout.fillWidth: true
                    }
                }
            }

            onActivated: function(index) {
                GeneralSettings.colorScheme = colorThemeSwitcher.textAt(index)
                GeneralSettings.save()
                HarunaApp.activateColorScheme(GeneralSettings.colorScheme)
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

            onActivated: function(index) {
                GeneralSettings.guiStyle = model.get(index).key
                HarunaApp.setGuiStyle(GeneralSettings.guiStyle)
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
                for (let i = 0; i < HarunaApp.availableGuiStyles().length; ++i) {
                    stylesModel.append({key: HarunaApp.availableGuiStyles()[i]})
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

        Item { Layout.preferredWidth: 1 }

        CheckBox {
            text: i18nc("@option:check", "Use Breeze icon theme")
            checked: GeneralSettings.useBreezeIconTheme
            onCheckedChanged: {
                GeneralSettings.useBreezeIconTheme = checked
                GeneralSettings.save()
            }

            ToolTip {
                text: i18nc("@info:tooltip", "Sets the icon theme to breeze.\nRequires restart.")
            }
        }

        Item {
            Layout.preferredWidth: Kirigami.Units.gridUnit
            Layout.preferredHeight: Kirigami.Units.gridUnit
        }
    }
}
