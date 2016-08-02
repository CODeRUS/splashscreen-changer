import QtQuick 2.0
import Sailfish.Silica 1.0
import org.nemomobile.dbus 2.0

Page {
    id: page
    allowedOrientations: Orientation.Portrait

    onStateChanged: {
        if (status == PageStatus.Active) {
            timer.start();
        }
    }

    function pictureSelected(filename) {
        console.log("###", filename)
        pageStack.push(Qt.resolvedUrl("ConfirmPage.qml"), {
            imageSource: filename,
            acceptCallback: confirmAction,
            acceptDestination: page,
            acceptDestinationAction: PageStackAction.Pop
        })
    }

    function confirmAction(filename) {
        console.log("###", filename)
        pageStack.pop(page, PageStackAction.Immediate)

        var fn = filename.toString().slice(7)
        dbus.call("flashPicture", [fn])
    }

    Timer {
        id: timer
        interval: 2000
        onTriggered: dbus.call("readPicture", [])
        running: true
    }

    SilicaFlickable {
        id: flick
        anchors.fill: parent
        contentHeight: img.height
        interactive: header.visible

        PullDownMenu {
            visible: header.visible
            MenuItem {
                text: "About"
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("AboutPage.qml"))
                }
            }
            MenuItem {
                text: "Select picture"
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("SecondPage.qml"), {
                        homePath: "/home/nemo",
                        showFormat: true,
                        title: "Select picture",
                        callback: pictureSelected
                    })
                }
            }
            MenuItem {
                text: "Refresh splashscreen"
                onClicked: {
                    dbus.call("readPicture", [])
                }
            }
        }

        PushUpMenu {
            visible: header.visible
            highlightColor: "red"
            MenuItem {
                text: "Reset splashscreen"
                onClicked: {
                    dbus.call("flashOriginal", [])
                }
            }
        }

        Image {
            id: img
            anchors.centerIn: parent
            width: Screen.width
            height: Screen.height
            sourceSize.width: width
            sourceSize.height: height
            cache: false
            fillMode: Image.PreserveAspectFit

            BusyIndicator {
                anchors.centerIn: parent
                size: BusyIndicatorSize.Large
                running: visible
                visible: img.status != Image.Ready
            }

            PageHeader {
                id: header
                title: "Splashscreen changer"
            }

            MouseArea {
                anchors.fill: parent
                onClicked: header.visible = !header.visible
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        anchors.margins: Theme.horizontalPageMargin
        color: Theme.highlightBackgroundColor
        visible: errorLabel.text

        Label {
            id: errorLabel
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }

        MouseArea {
            anchors.fill: parent
            onClicked: errorLabel.text = ""
        }
    }

    DBusInterface {
        id: dbus
        service: 'org.coderus.splashscreen.flasher'
        path: '/'
        iface: 'org.coderus.splashscreen.flasher'

        signalsEnabled: true

        signal pictureRead(string filename)
        signal someError(string errorMessage)

        onPictureRead: {
            console.log("###", filename)
            img.source = ""
            img.source = filename
        }

        onSomeError: {
            errorLabel.text = errorMessage
        }
    }
}


