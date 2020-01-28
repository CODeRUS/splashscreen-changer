import QtQuick 2.1
import Sailfish.Silica 1.0

Page {
    id: page
    objectName: "aboutPage"

    SilicaFlickable {
        id: flick
        anchors.fill: page
        contentHeight: column.height

        Column {
            id: column
            width: flick.width
            spacing: Theme.paddingLarge

            PageHeader {
                title: qsTr("Splashscreen Changer")
            }

            Label {
                text: "Splashscreen Flasher\nfor Jolla C and Intex Aqua Fish\nwritten by coderus in 0x7E0\nis dedicated to SailfishOS community"
                font.pixelSize: Theme.fontSizeMedium
                x: Theme.paddingLarge
                width: parent.width - Theme.paddingLarge * 2
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }

            Label {
                text: "Icon made by Ancelad"
                font.pixelSize: Theme.fontSizeMedium
                x: Theme.paddingLarge
                width: parent.width - Theme.paddingLarge * 2
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }

            Item { width: 1; height: Theme.paddingLarge }
        }

        VerticalScrollDecorator {}
    }
}
