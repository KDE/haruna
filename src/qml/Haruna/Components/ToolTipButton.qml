import QtQuick
import QtQuick.Controls

import org.kde.haruna

ToolButton {
    id: root

    required property string toolTipText
    property int toolTipWidth: 350

    icon.name: "documentinfo"
    checkable: true
    checked: false

    ToolTip {
        visible: root.checked
        delay: 0
        timeout: -1
        closePolicy: Popup.NoAutoClose

        contentItem: TextArea {
            text: root.toolTipText
            width: root.toolTipWidth
            background: Rectangle {
                color: "transparent"
            }
            readOnly: true
            textFormat: Text.RichText
            wrapMode: Text.WordWrap
            selectByMouse: true
            padding: 0
            onLinkActivated: function(link) {
                Qt.openUrlExternally(link)
            }
            onHoveredLinkChanged: hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
        }
    }
}
