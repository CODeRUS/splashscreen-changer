#include "rgb8.h"

#include <QDebug>

#include <QImage>

QString RGB8::convertImage(const QString &filename)
{
    QImage picture;
    picture.load(filename);
    if (picture.width() != 720 || picture.height() != 1280) {
        qDebug() << "Wrong image size" << picture.width() << picture.height();
        return QString();
    }

    picture = picture.convertToFormat(QImage::Format_RGB888);
    if (picture.save("/tmp/splashscreen-temp.ppm", "PPM")) {
        return "/tmp/splashscreen-temp.ppm";
    } else {
        qDebug() << "Error converting to PPM";
        return QString();
    }
}

