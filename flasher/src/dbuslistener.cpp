#include "L500DSplashscreen.hpp"
#include "T5Splashscreen.hpp"
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

namespace
{

bool gzipDecompress(QByteArray input, QByteArray &output)
{
    static int GZIP_WINDOWS_BIT = 15 + 16;
    static int GZIP_CHUNK_SIZE = 32 * 1024;

    // Prepare output
    output.clear();

    // Is there something to do?
    if (input.length() <= 0) {
        return true;
    }

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

}

DBusListener::DBusListener(QObject *parent) :
    QObject(parent)
{
    QDBusMessage ssu = QDBusMessage::createMethodCall(QStringLiteral("org.nemo.ssu"),
                                                      QStringLiteral("/org/nemo/ssu"),
                                                      QStringLiteral("org.nemo.ssu"),
                                                      QStringLiteral("deviceModel"));
    QDBusReply<QString> re = QDBusConnection::systemBus().call(ssu);
    m_model = re.value().toLower();

    qDebug() << Q_FUNC_INFO << m_model;

    if (m_model == QLatin1String("l500d")) {
        m_splashscreen = new L500DSplashscreen();
    } else if (m_model == QLatin1String("t5")) {
        m_splashscreen = new T5Splashscreen();
    }
}

void DBusListener::flashOriginal()
{
//    if (!m_splashscreen) {
//        errorMessage("Can't get device model");
//        return;
//    }

//    QFile zorig("/boot/splash.img.gz");
//    if (zorig.exists() && zorig.open(QFile::ReadOnly)) {
//        QByteArray gz = zorig.readAll();
//        zorig.close();
//        QByteArray img;
//        if (gzipDecompress(gz, img)) {
//            flashImage(img);
//            return;
//        } else {
//            errorMessage("Can't gunzip original splash.img.gz");
//        }
//    }

//    QFile orig("/boot/splash.img");
//    if (orig.exists() && orig.open(QFile::ReadOnly)) {
//        QByteArray img = orig.readAll();
//        orig.close();
//        qDebug() << "read /boot/splash.img" << img.size() << "bytes";

//        flashImage(img);
//    } else {
//        errorMessage("Can't read original splash.img");
//    }
}

void DBusListener::flashPicture(const QString &fileName)
{
    if (!m_splashscreen) {
        errorMessage("Can't get device model");
        return;
    }

    const QString errorString = m_splashscreen->writeSplashscreen(fileName);
    if (errorString.isEmpty()) {

    } else {
        errorMessage(errorString);
    }
}

void DBusListener::flashImage(const QByteArray &image)
{
//    QFile splashscreen("/dev/disk/by-partlabel/splash");
//    if (splashscreen.open(QFile::WriteOnly)) {
//        qDebug() << "writing" << image.size() << "bytes";
//        splashscreen.write(image);
//        splashscreen.close();

//        readPicture();
//    }
}

void DBusListener::readPicture()
{
    if (!m_splashscreen) {
        errorMessage("Can't get device model");
        return;
    }

    const QString fileName = m_splashscreen->readSplashscreen();
    if (QFileInfo::exists(fileName)) {
        emit pictureRead(fileName);
    } else {
        errorMessage(fileName);
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
    emit someError(message);
}
