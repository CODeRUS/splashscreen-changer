THEMENAME = sailfish-default

TARGET = splashscreen-changer

QT += dbus
CONFIG += sailfishapp

SOURCES += \
    src/splashscreen-changer.cpp

OTHER_FILES += \
    splashscreen-changer.desktop \
    translations/*.ts

CONFIG += sailfishapp_i18n

TRANSLATIONS += translations/splashscreen-changer-ru.ts

appicon.sizes = \
    86 \
    108 \
    128 \
    256

for(iconsize, appicon.sizes) {
    profile = $${iconsize}x$${iconsize}
    system(mkdir -p $${OUT_PWD}/$${profile})

    appicon.commands += /usr/bin/sailfish_svg2png \
        -s 1 1 1 1 1 1 $${iconsize} \
        $${_PRO_FILE_PWD_}/appicon \
        $${profile}/apps/ &&

    appicon.files += $${profile}
}
appicon.commands += true
appicon.path = /usr/share/icons/hicolor/

INSTALLS += appicon

CONFIG += sailfish-svg2png
