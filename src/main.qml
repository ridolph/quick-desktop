import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15

import uibase 1.0

FramelessWindow {
    id: root
    title: qsTr("QuickDesktop")
    width: 640
    height: 480
    minimumWidth: 320
    //maximumWidth: 640
    minimumHeight: 240
    //maximumHeight: 480
    radius: 6
    // 自定义系统按钮的显示隐藏
    windowFlags: Qt.WindowCloseButtonHint | Qt.WindowMinimizeButtonHint | Qt.WindowMaximizeButtonHint

    Rectangle {
        anchors.fill: parent
        color: "#2f2f2f"

        Rectangle {
            id: titleBar
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            height: 32
            color: "#1f1f1f"

            DragMoveWindowArea {
                enableDoubleClickedShowMax: true
                anchors.fill: parent
            }
        }

        Rectangle {
            anchors.centerIn: parent
            width: 100
            height: 100
            color: "red"
        }
    }
}
