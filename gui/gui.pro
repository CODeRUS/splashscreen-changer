TARGET = splashscreen-changer

CONFIG += sailfishapp

SOURCES += \
    src/splashscreen-changer.cpp

OTHER_FILES += \
    qml/splashscreen-changer.qml \
    qml/cover/CoverPage.qml \
    qml/pages/FirstPage.qml \
    qml/pages/SecondPage.qml \
    qml/pages/ConfirmPage.qml \
    qml/pages/AboutPage.qml \
    splashscreen-changer.desktop

SAILFISHAPP_ICONS = 86x86 108x108 128x128 256x256
