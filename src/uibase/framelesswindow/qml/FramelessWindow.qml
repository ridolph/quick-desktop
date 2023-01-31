import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15

import uibase 1.0

// 只是为了占位，mac/win都有特定实现
Window {
    id: root
    property int radius: 0
    property alias windowFlags: framelessHelper.windowFlags

    visible: true

    FramelessWindowHelper {
        id: framelessHelper
        Component.onCompleted: {
            // target赋值不能太早，否则root.flags属性还没赋值
            target = root
        }
    }
}
