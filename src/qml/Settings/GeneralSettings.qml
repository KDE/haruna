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
            text: i18nc("@label:spinbox", "OSD font size")
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
            onValueModified: {
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
                text: i18nc("@info:tooltip", "The default location of the open file dialog. If empty, " +
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
            onValueModified: {
                GeneralSettings.maxRecentFiles = maxRecentFiles.value
                GeneralSettings.save()
                recentFilesModel.populate()
            }

            ToolTip {
                text: i18nc("@info:tooltip", "The number of recent files to store. Enter 0 (zero) to disable.")
            }
        }

        Item { Layout.preferredWidth: 1 }

        RowLayout {
            CheckBox {
                text: i18nc("@option:check", "Allow only one instance")
                checked: GeneralSettings.useSingleInstance
                onClicked: {
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
                onClicked: {
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

        Item { Layout.preferredWidth: 1 }

        CheckBox {
            text: i18nc("@option:check", "Play new file")
            checked: GeneralSettings.playNewFileInSingleInstance
            enabled: GeneralSettings.useSingleInstance && GeneralSettings.appendVideoToSingleInstance
            onClicked: {
                GeneralSettings.playNewFileInSingleInstance = checked
                GeneralSettings.save()
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
            textRole: "text"
            valueRole: "value"
            model: ListModel {
                id: footerStyleModel
            }
            Component.onCompleted: {
                const defaultStyle = {
                    text: i18nc("@item:listbox the footer style", "Default"),
                    value: "default"
                }
                footerStyleModel.append(defaultStyle)

                const floatingStyle = {
                    text: i18nc("@item:listbox the footer style", "Floating"),
                    value: "floating"
                }
                footerStyleModel.append(floatingStyle)

                currentIndex = indexOfValue(GeneralSettings.footerStyle)
            }
            onActivated: function(index) {
                GeneralSettings.footerStyle = model.get(index).value
                GeneralSettings.save()
            }
        }

        Label {
            text: i18nc("@label:listbox", "Show floating footer on")
            Layout.alignment: Qt.AlignRight
        }

        RowLayout {
            ComboBox {
                textRole: "text"
                valueRole: "value"
                enabled: GeneralSettings.footerStyle === "floating"
                model: ListModel {
                    id: floatingFooterTriggerModel
                }

                Component.onCompleted: {
                    const everyMovement = {
                        text: i18nc("@item:listbox how to trigger the floating footer", "Every mouse movement"),
                        value: "EveryMouseMovement"
                    }
                    floatingFooterTriggerModel.append(everyMovement)

                    const bottomMovement = {
                        text: i18nc("@item:listbox how to trigger the floating footer", "Bottom mouse movement"),
                        value: "BottomMouseMovement"
                    }
                    floatingFooterTriggerModel.append(bottomMovement)

                    currentIndex = indexOfValue(GeneralSettings.floatingFooterTrigger)
                }
                onActivated: function(index) {
                    GeneralSettings.floatingFooterTrigger = model.get(index).value
                    GeneralSettings.save()
                }
            }

            ToolTipButton {
                toolTipText: i18nc("@info:tooltip",
                                   "What action shows the footer.<br>" +
                                   "<strong>Every mouse movement</strong> — every mouse movement over the video area<br>" +
                                   "<strong>Bottom mouse movement</strong> — mouse movement on the bottom of the video area")
            }

        }

        Label {
            text: i18nc("@label:spinbox", "Preview thumbnail")
            Layout.alignment: Qt.AlignRight
        }

        CheckBox {
            text: i18nc("@option:check", "Show preview thumbnail")
            checked: GeneralSettings.showPreviewThumbnail
            onClicked: {
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
                onClicked: {
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
            onValueModified: {
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
                onClicked: {
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
                onClicked: {
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
                onClicked: {
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
                                           ? i18nc("@info:tooltip extra wayland info for the “Remember window size and position” setting",
                                                   "<b>Restoring position is not supported on Wayland.</b><br><br>")
                                           : ""
                    text: i18nc("@info:tooltip", "Changes to the window’s size and position "
                                +"are saved and used for newly opened windows.<br><br>"
                                +"%1The “Resize to fit video” setting takes precedence.", waylandMessage)
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
            onClicked: {
                GeneralSettings.showMenuBar = checked
                GeneralSettings.save()
            }
        }

        Item { Layout.preferredWidth: 1 }

        CheckBox {
            text: i18nc("@option:check", "Show toolbar")
            checked: GeneralSettings.showHeader
            onClicked: {
                GeneralSettings.showHeader = checked
                GeneralSettings.save()
            }
        }

        Item { Layout.preferredWidth: 1 }

        CheckBox {
            text: i18nc("@option:check", "Show chapter markers")
            checked: GeneralSettings.showChapterMarkers
            onClicked: {
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

            textRole: "text"
            valueRole: "value"
            model: ListModel {
                id: stylesModel
            }

            onActivated: function(index) {
                GeneralSettings.guiStyle = model.get(index).value
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
                const defaultStyle = {
                    text: i18nc("@item:listbox default GUI style", "Default"),
                    value: "Default"
                }
                stylesModel.append(defaultStyle)


                // populate the model with the available styles
                const styles = HarunaApp.availableGuiStyles()
                for (let i = 0; i < styles.length; ++i) {
                    const style = {
                        text: styles[i],
                        value: styles[i]
                    }
                    stylesModel.append(style)
                }

                // set the saved style as the current item in the combo box
                currentIndex = indexOfValue(GeneralSettings.guiStyle)
            }
        }

        Item { Layout.preferredWidth: 1 }

        CheckBox {
            text: i18nc("@option:check", "Use Breeze icon theme")
            checked: GeneralSettings.useBreezeIconTheme
            onClicked: {
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
