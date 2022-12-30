import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.12

import uibase 1.0

Window {
    id: root

    default property alias data : contentArea.data

    width: 640
    height: 480
    // 如果设置了fixsize，内部忽略Qt.WindowMaximizeButtonHint
    minimumWidth: 320
    //maximumWidth: 640
    minimumHeight: 240
    //maximumHeight: 480
    visible: true
    title: qsTr("Hello World")
    flags: Qt.Window | Qt.FramelessWindowHint | Qt.WindowMaximizeButtonHint
    color: "transparent"

    function isMaximizedOrFullScreen() {
        return root.visibility === Window.Maximized || root.visibility === Window.FullScreen
    }

    // 使用单独Rectangle做边框，而不是直接用backgroundItem的border
    // 因为Rectangle的border会被子控件遮挡，还得设置子控件margins，
    // 有圆角的情况下OpacityMask会把边框裁掉
    Rectangle {
        id: backgroundBorderItem
        objectName: "backgroundBorderItem"
        anchors.fill: parent
        // 阴影宽度
        anchors.margins: isMaximizedOrFullScreen() ? 0 : 4
        // 圆角
        radius: isMaximizedOrFullScreen() ? 0 : 4
        // 边框颜色
        color: "black"
        Rectangle {
            id: backgroundItem
            anchors.fill: parent
            // 边框宽度
            anchors.margins: isMaximizedOrFullScreen() ? 0 : 0
            radius: parent.radius
            // 主背景色
            color: "#070709"
            // 默认父控件radius不会影响子控件，所以使用OpacityMask裁剪
            // 独立OpacityMask要求目标Item visible: false，会导致该Item的childAt都返回nullptr，鼠标事件也无法响应
            // visible: false
            // layer+OpacityMask完美实现圆角，不过Qt说有性能问题
            layer.enabled: true
            layer.effect: OpacityMask {
                maskSource: Rectangle {
                    width: backgroundItem.width
                    height: backgroundItem.height
                    radius: backgroundItem.radius
                    visible: false
                }
            }

            Item {
                id: contentArea
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: movableArea.bottom
                anchors.bottom: parent.bottom
            }

            Rectangle {
                id: movableArea
                // c++使用objectName访问qml对象
                objectName: "movableArea"
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                height: 36
                color: "transparent"

                Button {
                    x: 0
                    y: 10
                    width: 20
                    height: 20
                    onClicked: {
                        // 由于Qt官方bug， 最大化-最小化-恢复，窗口不能恢复成最大化，需要自己实现最小化
                        framelessHelper.targetShowMinimized()
                        //root.showMinimized()
                    }
                }
                Button {
                    x: 30
                    y: 10
                    width: 20
                    height: 20
                    onClicked: root.showMaximized()
                }
                Button {
                    x: 60
                    y: 10
                    width: 20
                    height: 20
                    onClicked: root.showNormal()
                }
                Button {
                    x: 90
                    y: 10
                    width: 20
                    height: 20
                    onClicked: root.close()
                }

                Rectangle {
                    id: imageButton
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter

                    width: 32
                    height: 32
                    color: "transparent"

                    Image {
                        anchors.centerIn: parent
                        fillMode: Image.PreserveAspectPad
                        source: "qrc:/res/framelesswindow/close.png"
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: parent.state = 'hover'
                        onExited: parent.state = 'normal'
                        onPressed: parent.state = 'normal'
                        onReleased: containsMouse ? parent.state = 'hover' : parent.state = 'normal'
                        onClicked: root.close()
                    }

                    states: [
                        State {
                            name: "normal"
                            PropertyChanges { target: imageButton; color: "transparent" }
                        },
                        State {
                            name: "hover"
                            PropertyChanges { target: imageButton; color: "red" }
                        }
                    ]
                }
            }

        }

        // 这种独立OpacityMask的方式需要backgroundItem visible: false才能生效
        // visible: false会导致Item的childAt都返回nullptr，鼠标事件也无法响应
        // 所以不采用
        /*
        OpacityMask {
            cached: true
            anchors.fill: backgroundItem
            source: backgroundItem
            maskSource: Rectangle {
                width: backgroundItem.width
                height: backgroundItem.height
                radius: backgroundItem.radius
                visible: false
            }
        }
        */
    }

    DropShadow {
        anchors.fill: backgroundBorderItem
        source: backgroundBorderItem
        radius: 4
        samples: 9
        cached: true
        color: "darkgray"
    }

    /*
    MouseArea{
        property real xmouse;   //鼠标的x轴坐标
        property real ymouse;   //y轴坐标
        anchors.fill: parent
        cursorShape: Qt.SizeAllCursor;

        onPressed: {
            xmouse=mouse.x;
            ymouse=mouse.y;
        }

        onPositionChanged: {
            root.x=root.x+(mouse.x-xmouse);
            root.y=root.y+(mouse.y-ymouse);
        }
    }
    */

    FramelessWindowHelper {
        id: framelessHelper
        Component.onCompleted: {
            // target赋值不能太早，否则root.flags属性还没赋值
            target = root
        }
    }
}
