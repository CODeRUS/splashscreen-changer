#include "dbuslistener.h"

#include <QDebug>
#include <QCoreApplication>
#include <QtDBus>

#include <QProcess>
#include <QByteArray>

#include <unistd.h>
#include <grp.h>
#include <pwd.h>

#include "src/rgb8.h"

DBusListener::DBusListener(QObject *parent) :
    QObject(parent)
{
}

void DBusListener::flashOriginal()
{
    QFile orig("/boot/splash.img");
    if (orig.open(QFile::ReadOnly)) {
        QByteArray img = orig.readAll();
        orig.close();
        qDebug() << "read /boot/splash.img" << img.size() << "bytes";

        flashImage(img);
    } else {
        errorMessage("Can't read original splash.img");
    }
}

void DBusListener::flashPicture(const QString &fileName)
{
    QString ppm = RGB8::convertImage(fileName);
    if (!ppm.isEmpty()) {
        QFile imgheader("/usr/share/splashscreen-changer/data/img.header");
        QByteArray imgHeader;
        if (imgheader.open(QFile::ReadOnly)) {
            imgHeader = imgheader.readAll();
            imgheader.close();
        }

        QFile file("/tmp/splashscreen-temp.ppm");
        if (file.open(QFile::ReadOnly)) {
            if (file.size() != 2764816) {
               errorMessage(QString("converted image size doesnt match 2764816 (%1)").arg(file.size()));
            }
            file.seek(16);

            QByteArray image;
            image.append(imgHeader);
            image.append(file.readAll());

            if (image.size() != 2765312) {
                errorMessage(QString("Image have wrong size %1").arg(image.size()));
            } else {
                flashImage(image);
            }

            file.close();
        }
    }
}

void DBusListener::flashImage(const QByteArray &image)
{
    QFile splashscreen("/dev/mmcblk0p16");
    if (splashscreen.open(QFile::WriteOnly)) {
        qDebug() << "writing" << image.size() << "bytes";
        splashscreen.write(image);
        splashscreen.close();

        readPicture();
    }
}

void DBusListener::readPicture()
{
    QByteArray img;

    QFile splashscreen("/dev/mmcblk0p16");
    if (splashscreen.open(QFile::ReadOnly)) {
        img = splashscreen.read(2765312);
        splashscreen.close();
        img = img.right(2764800);
    } else {
        errorMessage("Error reading splashscreen");
        return;
    }

    QFile ppmHeaderFile("/usr/share/splashscreen-changer/data/ppm.header");
    QFile splashscreenImage("/tmp/splashscreen-dump.ppm");
    if (splashscreenImage.open(QFile::WriteOnly)) {
        if (ppmHeaderFile.open(QFile::ReadOnly)) {
            splashscreenImage.write(ppmHeaderFile.readAll());
            splashscreenImage.write(img);

            ppmHeaderFile.close();

            qDebug() << "splashscreen picture read to /tmp/splashscreen-dump.ppm";

            Q_EMIT pictureRead("/tmp/splashscreen-dump.ppm");
        } else {
            errorMessage("Error opening ppm header");
        }

        splashscreenImage.close();
    } else {
        errorMessage("Error writingsplashscreen dump");
    }

//    QProcess flasher;
//    flasher.start("/bin/dd", QStringList() << "ibs=512" << "count=5401" << "if=/dev/mmcblk0p16" << "of=/tmp/splash-dump.img");
//    flasher.waitForFinished(10000);
//    if (flasher.state() == QProcess::Running) {
//        qDebug() << "Error reading splashscreen";
//        return;
//    } else {

//    }
}

void DBusListener::quit()
{
    qDebug() << "quit requested";
    QCoreApplication::instance()->quit();
}

void DBusListener::startService()
{
    qDebug("startService");

    bool serviceStarted = QDBusConnection::sessionBus().registerService("org.coderus.splashscreen.flasher");
    qDebug() << "DBus service" << (serviceStarted ? "registered" : "error!");

    bool objectRegistered = QDBusConnection::sessionBus().registerObject("/", this,
                            QDBusConnection::ExportScriptableSlots | QDBusConnection::ExportScriptableSignals);
    qDebug() << "DBus object" << (objectRegistered ? "registered" : "error!");

    if (serviceStarted && objectRegistered) {
        qDebug() << "listener started";
    } else {
        quit();
    }
}

void DBusListener::errorMessage(const QString &message)
{
    qDebug() << message;
    Q_EMIT someError(message);
}

