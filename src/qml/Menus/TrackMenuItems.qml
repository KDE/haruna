import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQml 2.12

Instantiator {
    id: root

    property bool isFirst: true
    property var menu

    signal subtitleChanged(int id, int index)

    model: 0
    onObjectAdded: function(index, object) {
        menu.insertItem( index, object )
    }
    onObjectRemoved: function(index, object) {
        menu.removeItem( object )
    }
    delegate: MenuItem {
        checkable: true
        checked: isFirst ? model.isFirstTrack : model.isSecondTrack
        text: model.text
        onTriggered: subtitleChanged(model.id, model.index)
    }
}
