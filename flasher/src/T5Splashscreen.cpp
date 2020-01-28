#include "T5Splashscreen.hpp"

#include <QBuffer>
#include <QFile>
#include <QImage>

namespace
{

QString s_partition = QStringLiteral("/dev/disk/by-partlabel/splash");

QSize s_size{1080, 2160};

int s_pictureOffset = 0x4000;
int s_pictureSize = 0x6AC9FA;

QByteArray convertImage(const QString &filename)
{
    QImage picture;
    picture.load(filename);
    if (picture.size().width() != s_size.width() || picture.size().height() != s_size.height()) {
        picture = picture.scaled(s_size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    }
    picture = picture.convertToFormat(QImage::Format_RGB888);
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    if (picture.save(&buffer, "BMP")) {
        return ba;
    } else {
        qDebug() << "Error converting image";
        return QByteArray();
    }
}

}

T5Splashscreen::T5Splashscreen()
{

}

QString T5Splashscreen::readSplashscreen() const
{
    QByteArray img;

    QFile splashscreen(s_partition);
    if (!splashscreen.open(QFile::ReadOnly)) {
        return QStringLiteral("Error opening splash partition!");
    }

    splashscreen.seek(s_pictureOffset);

    img = splashscreen.read(s_pictureSize);
    splashscreen.close();

    QFile splashscreenImage(QStringLiteral("/tmp/splashscreen-dump.bmp"));
    if (!splashscreenImage.open(QFile::WriteOnly)) {
        return QStringLiteral("Error opening temp file!");
    }
    splashscreenImage.write(img);
    splashscreenImage.close();

    return splashscreenImage.fileName();
}

QString T5Splashscreen::writeSplashscreen(const QString &fileName) const
{
    QByteArray data = convertImage(fileName);
    QFile tmp("/tmp/convert.bmp");
    tmp.open(QFile::WriteOnly);
    tmp.write(data);

    if (data.isEmpty()) {
        return QStringLiteral("Error converting image!");
    }

    QFile splashscreen(s_partition);
    if (!splashscreen.open(QFile::ReadWrite)) {
        return QStringLiteral("Error opening splash partition!");
    }

    if (!splashscreen.seek(s_pictureOffset)) {
        return QStringLiteral("Error seeking splash partition!");
    }

    qDebug()
        << Q_FUNC_INFO
        << "writing" << data.size() << "bytes";

    if (splashscreen.write(data) != data.size()) {
        return QStringLiteral("Error writing splash partition!");
    }

    splashscreen.close();

    return QString();
}

QSize T5Splashscreen::size() const
{
    return s_size;
}
