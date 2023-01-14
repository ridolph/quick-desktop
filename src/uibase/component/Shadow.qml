import QtQuick 2.15

BorderImage {
    property Item target;

    z: target.z - 1
    anchors.fill: target
    // 阴影宽度，由阴影图片决定
    // 在线生成阴影图片 http://inloop.github.io/shadow4android
    anchors { leftMargin: -25; topMargin: -25; rightMargin: -25; bottomMargin: -25 }
    // 九宫拉伸位置
    border { left: 30; top: 30; right: 30; bottom: 30 }
    source: "qrc:/res/component/shadow.png"
}

