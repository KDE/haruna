/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import org.kde.kirigami as Kirigami
import org.kde.haruna
import org.kde.haruna.settings

// Subtitles Folders
ColumnLayout {
    id: root

    // prevent creating multiple empty items
    // until the new one has been saved
    property bool canAddFolder: true

    implicitHeight: sectionTitle.height + sfListView.implicitHeight + sfAddFolder.height + 25

    RowLayout {
        Label {
            id: sectionTitle

            text: i18nc("@label; Below is a list of folders containing subtitles", "Load subtitles from")
        }
        ToolTipButton {
            toolTipText: i18nc("@info:tooltip;", "When a relative folder name starts with * (asterisk) "+
                               "and recursive search is enabled, all folders containing that name will be searched.\n"+
                               "Relative folders are relative to the playing file's folder.")
        }
    }

    ScrollView {
        Layout.preferredHeight: Math.min(Math.max(sfListView.contentHeight, 50), 200)
        Layout.fillWidth: true

        ListView {
            id: sfListView

            ScrollBar.vertical: ScrollBar { id: scrollBar }

            anchors.fill: parent
            spacing: Kirigami.Units.smallSpacing
            clip: true
            model: SubtitlesFoldersModel {
                id: subtitlesFoldersModel
            }

            delegate: ItemDelegate {
                id: sfDelegate

                required property int index
                required property string path

                width: ListView.view.width

                contentItem: Loader {
                    id: sfLoader
                    anchors.fill: parent
                    sourceComponent: sfDelegate.path === "" ? sfEditComponent : sfDisplayComponent
                }
                Component {
                    id: sfDisplayComponent

                    RowLayout {

                        Label {
                            id: sfLabel
                            text: sfDelegate.path
                            leftPadding: 10
                            Layout.fillWidth: true
                        }

                        Button {
                            icon.name: "edit-entry"
                            flat: true
                            onClicked: {
                                sfLoader.sourceComponent = sfEditComponent
                            }
                        }

                        Item { width: scrollBar.width }
                    }
                } // Component: display

                Component {
                    id: sfEditComponent

                    RowLayout {

                        TextField {
                            id: editField
                            leftPadding: 10
                            text: sfDelegate.path
                            Layout.leftMargin: 5
                            Layout.fillWidth: true
                            Component.onCompleted: editField.forceActiveFocus(Qt.MouseFocusReason)
                        }

                        Button {
                            property bool canDelete: editField.text === ""
                            icon.name: "delete"
                            flat: true
                            onClicked: {
                                if (!canDelete) {
                                    text = i18nc("@action:button", "Confirm Deletion")
                                    canDelete = true
                                    return
                                }

                                if (sfDelegate.index === sfListView.count - 1) {
                                    root.canAddFolder = true
                                }
                                subtitlesFoldersModel.deleteFolder(sfDelegate.index)
                            }
                            ToolTip {
                                text: i18nc("@info:tooltip", "Delete this folder from list")
                            }
                        }

                        Button {
                            icon.name: "dialog-ok"
                            flat: true
                            enabled: editField.text !== "" ? true : false
                            onClicked: {
                                subtitlesFoldersModel.updateFolder(editField.text, sfDelegate.index)
                                sfLoader.sourceComponent = sfDisplayComponent
                                if (sfDelegate.index === sfListView.count - 1) {
                                    root.canAddFolder = true
                                }
                            }
                            ToolTip {
                                text: i18nc("@info:tooltip", "Save changes")
                            }
                        }

                        Item { width: scrollBar.width }
                    }
                } // Component: edit
            } // ItemDelegate

            Label {
                text: i18n("No folder set")
                anchors.verticalCenter: parent.verticalCenter
                visible: sfListView.count === 0
                opacity: 0.7
            }
        }
    }

    Item {
        id: spacer

        Layout.preferredHeight: 5
    }

    Button {
        id: sfAddFolder

        icon.name: "list-add"
        text: i18nc("@action:button", "Add New Folder")
        enabled: root.canAddFolder
        onClicked: {
            subtitlesFoldersModel.addFolder()
            root.canAddFolder = false
        }

        ToolTip.text: i18nc("@info:tooltip add new subtitle folder setting",
                            "Add a folder in which to look for subtitles.")
        ToolTip.visible: hovered
        ToolTip.delay: 700
    }

    RowLayout {
        CheckBox {
            id: recursiveSubtitlesSearch

            text: i18nc("@label:check", "Search subtitles recursively")
            checked: SubtitlesSettings.recursiveSubtitlesSearch

            onClicked: {
                SubtitlesSettings.recursiveSubtitlesSearch = checked
                SubtitlesSettings.save()
            }
        }

        ToolButton {
            icon.name: "documentinfo"
            checkable: true
            checked: false
            Layout.preferredHeight: Kirigami.Units.iconSizes.medium

            ToolTip {
                text: i18nc("@info:tooltip",
                            "Subtitles are searched recursively in the folders defined by the “%1” setting.\n" +
                            "Only relative folders are searched.", sectionTitle.text)
                visible: (parent as ToolButton).checked
                delay: 0
                timeout: -1
                closePolicy: Popup.NoAutoClose
            }
        }
    }
    Item { Layout.preferredWidth: 1; Layout.preferredHeight: 10 }
}
