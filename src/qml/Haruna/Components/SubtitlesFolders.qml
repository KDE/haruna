/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import org.kde.kirigami as Kirigami

// Subtitles Folders
Item {
    id: root

    // prevent creating multiple empty items
    // until the new one has been saved
    property bool canAddFolder: true

    implicitHeight: sectionTitle.height + sfListView.implicitHeight + sfAddFolder.height + 25

    Label {
        id: sectionTitle

        text: i18nc("@title", "Load subtitles from")
        bottomPadding: 10
    }

    ListView {
        id: sfListView
        property int sfDelegateHeight: 40

        anchors.top: sectionTitle.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        implicitHeight: count > 5
                ? 5 * sfListView.sfDelegateHeight + (sfListView.spacing * 4)
                : count * sfListView.sfDelegateHeight + (sfListView.spacing * (count - 1))
        spacing: 5
        clip: true
        model: subtitlesFoldersModel
        Layout.fillWidth: true
        ScrollBar.vertical: ScrollBar { id: scrollBar }
        delegate: Rectangle {
            id: sfDelegate

            required property int index
            required property string display

            width: root.width
            height: sfListView.sfDelegateHeight
            color: Kirigami.Theme.alternateBackgroundColor

            Loader {
                id: sfLoader
                anchors.fill: parent
                sourceComponent: sfDelegate.display === "" ? sfEditComponent : sfDisplayComponent
            }

            Component {
                id: sfDisplayComponent

                RowLayout {

                    Label {
                        id: sfLabel
                        text: sfDelegate.display
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
                        text: sfDelegate.display
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
                                text = i18nc("@action:button", "Confirm deletion")
                                canDelete = true
                                return
                            }

                            if (sfDelegate.index === sfListView.count - 1) {
                                root.canAddFolder = true
                            }
                            subtitlesFoldersModel.deleteFolder(sfDelegate.index)
                            const rows = sfListView.count
                            sfListView.implicitHeight = rows > 5
                                    ? 5 * sfListView.sfDelegateHeight + (sfListView.spacing * 4)
                                    : rows * sfListView.sfDelegateHeight + (sfListView.spacing * (rows - 1))

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
        } // delegate: Rectangle
    }

    Item {
        id: spacer
        anchors.top: sfListView.bottom
        height: 5
    }

    Button {
        id: sfAddFolder

        anchors.left: parent.left
        anchors.top: spacer.bottom
        icon.name: "list-add"
        text: i18nc("@action:button", "Add new folder")
        enabled: root.canAddFolder
        onClicked: {
            subtitlesFoldersModel.addFolder()
            const rows = sfListView.count
            sfListView.implicitHeight = rows > 5
                    ? 5 * sfListView.sfDelegateHeight + (sfListView.spacing * 4)
                    : rows * sfListView.sfDelegateHeight + (sfListView.spacing * (rows - 1))
            root.canAddFolder = false
        }

        ToolTip.text: i18nc("@info:tooltip add new sutbtitle folder setting",
                            "Add a folder in which to look for subtitles.")
        ToolTip.visible: hovered
        ToolTip.delay: 700
    }
}
