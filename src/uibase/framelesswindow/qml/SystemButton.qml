import QtQuick 2.15
import QtQuick.Controls 2.15

Button {
    property color hoveredColor: "grey"
    property alias image: bgImage.source

    id: button

    background: Rectangle {
        radius: 4
        color: (button.hovered && !button.down) ? hoveredColor : "transparent"

        Image {
            id: bgImage
            anchors.centerIn: parent
            fillMode: Image.PreserveAspectFit            
        }
    }
}
