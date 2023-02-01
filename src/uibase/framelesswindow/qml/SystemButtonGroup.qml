import QtQuick 2.15
import QtQuick.Window 2.15

Row {
    property int buttonWidth: 32
    property int buttonheight: 32
    property bool minimizeButtonVisible: true
    property bool maximizeButtonVisible: true
    property bool closeButtonVisible: true

    signal minimizeButtonClicked
    signal maximizeButtonClicked
    signal restoreButtonClicked
    signal closeButtonClicked

    id: root
    anchors.right: parent.right
    anchors.top: parent.top
    spacing: 2
    // 最大整数，保证在最上面
    z: 2147483647

    SystemButton {
        width: root.buttonWidth
        height: root.buttonheight
        hoveredColor: "#37393b"
        image: "qrc:/res/framelesswindow/minimize.png"

        visible: root.minimizeButtonVisible

        onClicked: root.minimizeButtonClicked()
    }
    SystemButton {
        width: root.buttonWidth
        height: root.buttonheight
        hoveredColor: "#37393b"
        image: "qrc:/res/framelesswindow/maximize.png"

        visible: root.maximizeButtonVisible && Window.window.visibility === Window.Windowed

        onClicked: root.maximizeButtonClicked()
    }
    SystemButton {
        width: root.buttonWidth
        height: root.buttonheight
        hoveredColor: "#37393b"
        image: "qrc:/res/framelesswindow/restore.png"

        visible: root.maximizeButtonVisible && (Window.window.visibility === Window.Maximized | Window.window.visibility === Window.FullScreen)

        onClicked: root.restoreButtonClicked();
    }
    SystemButton {
        width: root.buttonWidth
        height: root.buttonheight
        hoveredColor: "red"
        image: "qrc:/res/framelesswindow/close.png"

        visible: root.closeButtonVisible

        onClicked: root.closeButtonClicked()
    }
}
