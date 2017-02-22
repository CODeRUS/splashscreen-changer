#include "dbuslistener.h"

#include <QDebug>
#include <QCoreApplication>
#include <QtDBus>

#include <QProcess>
#include <QByteArray>

#include <unistd.h>
#include <grp.h>
#include <pwd.h>

#include <zlib.h>

#include "src/rgb8.h"

DBusListener::DBusListener(QObject *parent) :
    QObject(parent)
{
}

void DBusListener::flashOriginal()
{
    QFile zorig("/boot/splash.img.gz");
    if (zorig.exists() && zorig.open(QFile::ReadOnly)) {
        QByteArray gz = zorig.readAll();
        zorig.close();
        QByteArray img;
        if (gzipDecompress(gz, img)) {
            flashImage(img);
            return;
        } else {
            errorMessage("Can't gunzip original splash.img.gz");
        }
    }

    QFile orig("/boot/splash.img");
    if (orig.exists() && orig.open(QFile::ReadOnly)) {
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
    QByteArray ppm = RGB8::convertImage(fileName);
    if (!ppm.isEmpty()) {
        QFile imgheader("/usr/share/splashscreen-changer/data/img.header");
        QByteArray imgHeader;
        if (imgheader.open(QFile::ReadOnly)) {
            imgHeader = imgheader.readAll();
            imgheader.close();
        }

        if (ppm.size() != 2764816) {
            errorMessage(QString("converted image size doesnt match 2764816 (%1)").arg(ppm.size()));
        }
        else {
            QByteArray image;
            image.append(imgHeader);
            image.append(ppm.right(2764800));

            if (image.size() != 2765312) {
                errorMessage(QString("Image have wrong size %1").arg(image.size()));
            } else {
                flashImage(image);
            }
        }
    }
}

void DBusListener::flashImage(const QByteArray &image)
{
    QFile splashscreen("/dev/disk/by-partlabel/splash");
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

    QFile splashscreen("/dev/disk/by-partlabel/splash");
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

            QByteArray swapped = RGB8::convertImage("/tmp/splashscreen-dump.ppm", "PPM");
            splashscreenImage.resize(0);
            splashscreenImage.seek(0);
            splashscreenImage.write(swapped);
            splashscreenImage.close();

            Q_EMIT pictureRead("/tmp/splashscreen-dump.ppm");
        } else {

            splashscreenImage.close();
            errorMessage("Error opening ppm header");
        }
    } else {
        errorMessage("Error writing splashscreen dump");
    }
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

bool DBusListener::gzipDecompress(QByteArray input, QByteArray &output)
{
    int GZIP_WINDOWS_BIT = 15 + 16;
    int GZIP_CHUNK_SIZE = 32 * 1024;

    // Prepare output
    output.clear();

    // Is there something to do?
    if (input.length() > 0) {
        // Prepare inflater status
        z_stream strm;
        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;
        strm.opaque = Z_NULL;
        strm.avail_in = 0;
        strm.next_in = Z_NULL;

        // Initialize inflater
        int ret = inflateInit2(&strm, GZIP_WINDOWS_BIT);

        if (ret != Z_OK) {
            return false;
        }

        // Extract pointer to input data
        char *input_data = input.data();
        int input_data_left = input.length();

        // Decompress data until available
        do {
            // Determine current chunk size
            int chunk_size = qMin(GZIP_CHUNK_SIZE, input_data_left);

            // Check for termination
            if(chunk_size <= 0)
                break;

            // Set inflater references
            strm.next_in = (unsigned char*)input_data;
            strm.avail_in = chunk_size;

            // Update interval variables
            input_data += chunk_size;
            input_data_left -= chunk_size;

            // Inflate chunk and cumulate output
            do {

                // Declare vars
                char out[GZIP_CHUNK_SIZE];

                // Set inflater references
                strm.next_out = (unsigned char*)out;
                strm.avail_out = GZIP_CHUNK_SIZE;

                // Try to inflate chunk
                ret = inflate(&strm, Z_NO_FLUSH);

                switch (ret) {
                case Z_NEED_DICT:
                    ret = Z_DATA_ERROR;
                case Z_DATA_ERROR:
                case Z_MEM_ERROR:
                case Z_STREAM_ERROR:
                    // Clean-up
                    inflateEnd(&strm);

                    // Return
                    return(false);
                }

                // Determine decompressed size
                int have = (GZIP_CHUNK_SIZE - strm.avail_out);

                // Cumulate result
                if(have > 0) {
                    output.append((char*)out, have);
                }

            } while (strm.avail_out == 0);

        } while (ret != Z_STREAM_END);

        // Clean-up
        inflateEnd(&strm);

        // Return
        return (ret == Z_STREAM_END);
    }
    else
        return true;
}

