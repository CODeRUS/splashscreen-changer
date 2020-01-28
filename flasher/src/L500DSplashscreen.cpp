#include "L500DSplashscreen.hpp"

#include <QBuffer>
#include <QFile>
#include <QImage>
#include <QSize>

namespace
{

QString s_partition = QStringLiteral("/dev/disk/by-partlabel/splash");

QSize s_size{720, 1280};
int s_dataSize = 2764800;
int s_pictureSize = 2764816;
int s_imageSize = 2765312;

QByteArray convertImage(const QString &filename)
{
    QImage picture;
    picture.load(filename);
    if (picture.size().width() != s_size.width() || picture.size().height() != s_size.height()) {
        picture = picture.scaled(s_size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    }
    picture = picture.convertToFormat(QImage::Format_Indexed8);
    picture = picture.rgbSwapped();
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    if (picture.save(&buffer, "PPM")) {
        return ba;
    } else {
        qDebug() << "Error converting image";
        return QByteArray();
    }
}

}

L500DSplashscreen::L500DSplashscreen()
{

}

QString L500DSplashscreen::readSplashscreen() const
{
    QByteArray img;

    QFile splashscreen(s_partition);
    if (!splashscreen.open(QFile::ReadOnly)) {
        return QStringLiteral("Error opening splash partition!");
    }

    img = splashscreen.read(s_imageSize);
    splashscreen.close();
    img = img.right(s_dataSize);

    QFile ppmHeaderFile(QStringLiteral("/usr/share/splashscreen-changer/data/ppm.header"));
    QFile splashscreenImage(QStringLiteral("/tmp/splashscreen-dump.ppm"));
    if (!splashscreenImage.open(QFile::WriteOnly)) {
        return QStringLiteral("Error opening temp file!");
    }

    if (!ppmHeaderFile.open(QFile::ReadOnly)) {
        return QStringLiteral("Error opening ppm header file!");
    }

    splashscreenImage.write(ppmHeaderFile.readAll());
    splashscreenImage.write(img);

    ppmHeaderFile.close();

    QByteArray swapped = convertImage(splashscreenImage.fileName());
    splashscreenImage.resize(0);
    splashscreenImage.seek(0);
    splashscreenImage.write(swapped);
    splashscreenImage.close();

    return splashscreenImage.fileName();
}

QString L500DSplashscreen::writeSplashscreen(const QString &fileName) const
{
    QByteArray data = convertImage(fileName);
    if (data.isEmpty()) {
        return QStringLiteral("Error converting image!");
    }
    if (data.size() != s_pictureSize) {
        return QStringLiteral("converted image size doesnt match %1 (%2)").arg(s_pictureSize).arg(data.size());
    }

    QFile imgheader(QStringLiteral("/usr/share/splashscreen-changer/data/l500d.header"));
    QByteArray imgHeader;
    if (!imgheader.open(QFile::ReadOnly)) {
        return QStringLiteral("Error opening image header file!");
    }
    imgHeader = imgheader.readAll();
    imgheader.close();

    QByteArray image;
    image.append(imgHeader);
    image.append(data.right(s_dataSize));

    if (image.size() != s_imageSize) {
        return QStringLiteral("Image have wrong size %1").arg(image.size());
    }

    QFile splashscreen(s_partition);
    if (!splashscreen.open(QFile::WriteOnly)) {
        return QStringLiteral("Error opening splash partition!");
    }

    qDebug()
        << Q_FUNC_INFO
        << "writing" << image.size() << "bytes";
    splashscreen.write(image);
    splashscreen.close();

    return QString();
}

QSize L500DSplashscreen::size() const
{
    return s_size;
}
