/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import org.kde.kirigami 2.11 as Kirigami

// Subtitles Folders
Item {
    id: root

    // prevent creating multiple empty items
    // until the new one has been saved
    property bool canAddFolder: true

    Layout.fillWidth: true
    implicitHeight: sectionTitle.height + sfListView.implicitHeight + sfAddFolder.height + 25

    Label {
        id: sectionTitle

        text: i18n("Load subtitles from")
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
        model: subsFoldersModel
        Layout.fillWidth: true
        ScrollBar.vertical: ScrollBar { id: scrollBar }
        delegate: Rectangle {
            id: sfDelegate
            width: root.width
            height: sfListView.sfDelegateHeight
            color: Kirigami.Theme.alternateBackgroundColor

            Loader {
                id: sfLoader
                anchors.fill: parent
                sourceComponent: model.display === "" ? sfEditComponent : sfDisplayComponent
            }

            Component {
                id: sfDisplayComponent

                RowLayout {

                    Label {
                        id: sfLabel
                        text: model.display
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
                        text: model.display
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
                                text = i18n("Confirm deletion")
                                canDelete = true
                                return
                            }

                            if (model.row === sfListView.count - 1) {
                                root.canAddFolder = true
                            }
                            subsFoldersModel.deleteFolder(model.row)
                            const rows = sfListView.count
                            sfListView.implicitHeight = rows > 5
                                    ? 5 * sfListView.sfDelegateHeight + (sfListView.spacing * 4)
                                    : rows * sfListView.sfDelegateHeight + (sfListView.spacing * (rows - 1))

                        }
                        ToolTip {
                            text: i18n("Delete this folder from list")
                        }
                    }

                    Button {
                        icon.name: "dialog-ok"
                        flat: true
                        enabled: editField.text !== "" ? true : false
                        onClicked: {
                            subsFoldersModel.updateFolder(editField.text, model.row)
                            sfLoader.sourceComponent = sfDisplayComponent
                            if (model.row === sfListView.count - 1) {
                                root.canAddFolder = true
                            }
                        }
                        ToolTip {
                            text: i18n("Save changes")
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
        anchors.top: spacer.bottom
        icon.name: "list-add"
        text: i18n("Add new folder")
        enabled: root.canAddFolder
        onClicked: {
            subsFoldersModel.addFolder()
            const rows = sfListView.count
            sfListView.implicitHeight = rows > 5
                    ? 5 * sfListView.sfDelegateHeight + (sfListView.spacing * 4)
                    : rows * sfListView.sfDelegateHeight + (sfListView.spacing * (rows - 1))
            root.canAddFolder = false
        }
    }
}
