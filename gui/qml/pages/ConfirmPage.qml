import QtQuick 2.0
import Sailfish.Silica 1.0

Dialog {
    id: dialog
    allowedOrientations: Orientation.Portrait
    canAccept: img.okay

    property alias imageSource: img.source
    property var acceptCallback

    onAccepted: {
        if (typeof acceptCallback == "function") {
            acceptCallback(img.source);
        }
    }

    Image {
        id: img
        anchors.centerIn: parent
        width: Screen.width
        height: Screen.height
        cache: false
        fillMode: Image.PreserveAspectFit
        property bool okay: status == Image.Ready
                            && sourceSize.width === Screen.width
                            & sourceSize.height === Screen.height

        BusyIndicator {
            anchors.centerIn: parent
            size: BusyIndicatorSize.Large
            running: visible
            visible: img.status != Image.Ready
        }
    }

    DialogHeader {
        acceptText: "Confirm"
        cancelText: "Back"
    }

    Rectangle {
        anchors.bottom: dialog.bottom
        width: dialog.width
        height: Theme.itemSizeSmall
        color: Theme.rgba(img.okay ? "green" : "red", Theme.highlightBackgroundOpacity)

        Label {
            anchors.fill: parent
            anchors.margins: Theme.paddingLarge
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.NoWrap
            text: "Image: %1x%2  Need: %3x%4  %5".arg(img.sourceSize.width).arg(img.sourceSize.height)
                                                 .arg(Screen.width).arg(Screen.height)
                                                 .arg(img.okay ? "Valid" : "Invalid")
        }
    }
}

