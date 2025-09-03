import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import org.kde.kirigami 2.11 as Kirigami

Popup {
    id: root

    property string headerTitle

    signal actionSelected(string actionName)

    implicitHeight: parent.height * 0.9
    implicitWidth: parent.width* 0.9
    modal: true
    anchors.centerIn: parent
    focus : true

    onOpened: {
        filterActionsField.text = ""
        filterActionsField.focus = true
    }

    onActionSelected: close()

    Action {
        shortcut: "ctrl+f"
        onTriggered: filterActionsField.forceActiveFocus(Qt.ShortcutFocusReason)
    }

    ColumnLayout {
        anchors.fill: parent

        Kirigami.Heading {
            text: root.headerTitle
        }

        Label {
            text: qsTr("Double click to set action")
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignTop
        }

        TextField {
            id: filterActionsField

            placeholderText: qsTr("Type to filter...")
            focus: true
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignTop
            KeyNavigation.down: actionNone
            onTextChanged: {
                const menuModel = actionsListView.actionsList
                actionsListView.model = menuModel.filter(action => action.toLowerCase().includes(text))
            }
        }
        Button {
            id: actionNone

            Layout.fillWidth: true
            text: qsTr("Clear current action")
            KeyNavigation.up: filterActionsField
            KeyNavigation.down: actionsListView
            onClicked: actionSelected("")
            Keys.onEnterPressed: actionSelected("")
            Keys.onReturnPressed: actionSelected("")
        }

        ListView {
            id: actionsListView

            property var actionsList: Object.keys(actions.list).sort()

            implicitHeight: 30 * model.count
            model: actionsList
            spacing: 1
            clip: true
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignTop
            currentIndex: focus ? 0 : -1
            delegate: ItemDelegate {
                height: 30
                width: parent.width
                text: modelData
                onDoubleClicked: actionSelected(modelData)
                Keys.onEnterPressed: actionSelected(modelData)
                Keys.onReturnPressed: actionSelected(modelData)
            }
        }
    }
}
