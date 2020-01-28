TARGET = splashscreen-flasher
target.path = /usr/bin

QT += dbus
CONFIG += link_pkgconfig
PKGCONFIG += zlib

SOURCES += \
    src/L500DSplashscreen.cpp \
    src/main.cpp \
    src/dbuslistener.cpp

HEADERS += \
    src/ISplashscreen.hpp \
    src/L500DSplashscreen.hpp \
    src/dbuslistener.h

headers.files += \
    data/ppm.header \
    data/l500d.header
headers.path = /usr/share/splashscreen-changer/data

dbus.files += dbus/org.coderus.splashscreen.flasher.service
dbus.path = /usr/share/dbus-1/services

INSTALLS = target headers dbus

