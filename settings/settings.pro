TEMPLATE = aux

qml.files = \
    FirstPage.qml \
    SecondPage.qml \
    ConfirmPage.qml \
    AboutPage.qml
qml.path = /usr/share/splashscreen-changer/qml

settings.files = splashscreen.json
settings.path = /usr/share/jolla-settings/entries

icon.files = icon-m-splashscreenchanger.png
icon.path = /usr/share/splashscreen-changer/icons

INSTALLS = qml settings icon
