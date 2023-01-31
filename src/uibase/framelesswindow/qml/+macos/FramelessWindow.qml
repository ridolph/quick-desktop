import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15

import uibase 1.0

Window {
    id: root

    // mac下没用，只是为了兼容
    property int radius: 0
    // 自己实现交通灯的显示隐藏，不用Qt的实现（Qt只是禁用，没有隐藏）
    property alias windowFlags: framelessHelper.windowFlags
   
    // 默认属性：外部使用FramelessWindow时，没有赋值给任何属性的对象都会赋值给默认变量
    // data属性类似与children属性，保存所有子控件
    // 通过默认属性+data别名的方式，将外部使用FramelessWindow时的子控件赋值给contentArea
    default property alias data: contentArea.data

    flags: Qt.Window
    visible: true

    Item {
        id: contentArea
        anchors.fill: parent
    }

    FramelessWindowHelper {
        id: framelessHelper
        Component.onCompleted: {
            // target赋值不能太早，否则root.flags属性还没赋值
            target = root
        }
    }
}
