import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtGraphicalEffects 1.15

import uibase 1.0

import "../../component"

Window {
    id: root

    property int radius: 4
    property alias windowFlags: framelessHelper.windowFlags

    // 暂时用不到边框
    // property int borderWidth: 0
    // property color borderColor: "black"

    // 默认属性：外部使用FramelessWindow时，没有赋值给任何属性的对象都会赋值给默认变量
    // data属性类似与children属性，保存所有子控件
    // 通过默认属性+data别名的方式，将外部使用FramelessWindow时的子控件赋值给contentArea
    default property alias data: contentArea.data

    flags: Qt.Window | Qt.FramelessWindowHint | Qt.WindowMaximizeButtonHint
    color: "transparent"
    visible: true

    enum ShadowType {
        SystemShadow,   // 系统阴影：不支持窗口圆角，不能修改阴影样式，性能最好
        PictureShadow,  // 图片阴影：修改阴影样式需要替换图片
        CodeShadow      // 代码绘制阴影：代码调整阴影样式，性能最差
    }

    // 不定义在顶级外部就不能访问了
    QtObject {
        id: internal

        readonly property int shadowType: FramelessWindow.ShadowType.PictureShadow
    }

    function __getFlags() {
        if (FramelessWindow.ShadowType.SystemShadow === internal.shadowType) {
            return 0;
        }

        return __isMaximizedOrFullScreen() ? 0 : root.radius;
    }
    function __getRadius() {
        if (FramelessWindow.ShadowType.SystemShadow === internal.shadowType) {
            return 0;
        }

        return __isMaximizedOrFullScreen() ? 0 : root.radius;
    }
    function __getShadowWidth() {
        if (FramelessWindow.ShadowType.SystemShadow === internal.shadowType) {
            return 0;
        }

        return __isMaximizedOrFullScreen() ? 0 : 25;
    }
    function __isMaximizedOrFullScreen() {
        return root.visibility === Window.Maximized || root.visibility === Window.FullScreen
    }

    // 使用单独Rectangle做边框，而不是直接用backgroundItem的border
    // 因为Rectangle的border会被子控件遮挡，还得设置子控件margins，
    // 有圆角的情况下OpacityMask会把边框裁掉
    Rectangle {
        id: backgroundShadowItem
        objectName: "backgroundShadowItem"
        anchors.fill: parent
        // 阴影宽度
        anchors.margins:__getShadowWidth()
        // 圆角
        radius: __getRadius()
        // 边框颜色
        //color: root.borderColor
        Rectangle {
            id: backgroundItem
            anchors.fill: parent
            // 边框宽度
            // anchors.margins: __isMaximizedOrFullScreen() ? 0 : root.borderWidth
            anchors.margins: 0
            radius: backgroundShadowItem.radius
            // 主背景色
            color: "#000000"

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
                anchors.fill: parent

                SystemButtonGroup {
                    onMinimizeButtonClicked: {
                        // windows平台由于Qt官方bug， 最大化-最小化-恢复，窗口不能恢复成最大化，需要自己实现最小化
                        framelessHelper.targetShowMinimized()
                        //Window.window.showMinimized()
                    }
                    onMaximizeButtonClicked: Window.window.showMaximized()
                    onRestoreButtonClicked: Window.window.showNormal()
                    onCloseButtonClicked: Window.window.close()
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

    Shadow {
        target: backgroundShadowItem
        visible: FramelessWindow.ShadowType.PictureShadow === internal.shadowType ? true : false
    }

    DropShadow {
        anchors.fill: backgroundShadowItem
        source: backgroundShadowItem
        radius: 20
        samples: 41
        cached: true
        color: "#555555"
        visible: FramelessWindow.ShadowType.CodeShadow === internal.shadowType ? true : false
    }

    /*
    // 这种方式的窗口拖动没有window snap&mac窗口边缘吸附效果
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
        systemShadow: FramelessWindow.ShadowType.SystemShadow === internal.shadowType ? true : false
        Component.onCompleted: {
            // target赋值不能太早，否则root.flags属性还没赋值
            target = root
        }
    }
}
