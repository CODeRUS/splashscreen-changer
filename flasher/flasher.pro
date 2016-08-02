TARGET = splashscreen-flasher
target.path = /usr/bin

QT += dbus

SOURCES += \
    src/main.cpp \
    src/rgb8.cpp \
    src/dbuslistener.cpp

HEADERS += \
    src/rgb8.h \
    src/dbuslistener.h

headers.files += \
    data/ppm.header \
    data/img.header
headers.path = /usr/share/splashscreen-changer/data

dbus.files += dbus/org.coderus.splashscreen.flasher.service
dbus.path = /usr/share/dbus-1/services

INSTALLS = target headers dbus

