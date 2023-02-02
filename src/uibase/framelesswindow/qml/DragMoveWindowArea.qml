import QtQuick 2.15
import QtQuick.Window 2.15

Item {
    property bool enableDoubleClickedShowMax: true

    id: root

    MouseArea {
        anchors.fill: parent
        onPressed: Window.window.startSystemMove()
        onDoubleClicked: {
            if (!root.enableDoubleClickedShowMax) {
                return
            }

            if (Window.window.visibility === Window.Windowed) {
                Window.window.showMaximized()
            } else {
                Window.window.showNormal()
            }
        }
    }
}
