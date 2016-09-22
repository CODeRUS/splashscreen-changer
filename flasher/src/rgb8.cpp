#include "rgb8.h"

#include <QDebug>

#include <QImage>
#include <QBuffer>

QByteArray RGB8::convertImage(const QString &filename, const char *format)
{
    QImage picture;
    picture.load(filename, format);
    if (picture.width() != 720 || picture.height() != 1280) {
        qDebug() << "Wrong image size" << picture.width() << picture.height();
        return QByteArray();
    }

    picture = picture.convertToFormat(QImage::Format_Indexed8);
    picture = picture.rgbSwapped();
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    if (picture.save(&buffer, "PPM")) {
        return ba;
    } else {
        qDebug() << "Error converting to PPM";
        return QByteArray();
    }
}

