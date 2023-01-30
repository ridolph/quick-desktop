import QtQuick 2.15
import QtQuick.Window 2.15

import uibase 1.0

FramelessWindow {
    title: qsTr("QuickDesktop")
    width: 640
    height: 480
    minimumWidth: 320
    //maximumWidth: 640
    minimumHeight: 240
    //maximumHeight: 480
    radius: 6

    DragHandler {
        onActiveChanged: {if (active) root.startSystemMove();}
        target: null
    }

    Rectangle {
        anchors.fill: parent
        color: "green"

        Rectangle {
            anchors.centerIn: parent
            width: 100
            height: 100
            color: "red"
        }
    }
}
