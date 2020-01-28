import QtQuick 2.0
import Sailfish.Silica 1.0

CoverBackground {
    Item {
        anchors.fill: parent

        Image {
            id : coverImage
            width: Theme.iconSizeExtraLarge
            height: Theme.iconSizeExtraLarge
            sourceSize.width: width
            sourceSize.height: height
            anchors.centerIn: parent
            source: "image://theme/splashscreen-changer"
        }

        Label {
            id: label
            anchors.top: coverImage.bottom
            anchors.horizontalCenter: coverImage.horizontalCenter
            anchors.topMargin: 10
            text: qsTr("Splashscreen changer")
        }
    }
}
