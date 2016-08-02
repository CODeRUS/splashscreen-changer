#include "rgb8.h"

#include <QDebug>

RGB8::RGB8(QObject *parent) : QObject(parent)
{

}

void RGB8::toRawData(const QString &filename)
{
    QImage image;
    image.load(filename);
    if (image.width() != 720 || image.height() != 1280) {
        qDebug() << "Wrong image size" << image.width() << image.height();
        return;
    }

    image = image.convertToFormat(QImage::Format_RGB888);
    image.save("/home/nemo/test.rgb", "PPM");
}

