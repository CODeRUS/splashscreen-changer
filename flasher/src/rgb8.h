#ifndef RGB8_H
#define RGB8_H

#include <QString>

class RGB8
{
public:
    static QByteArray convertImage(const QString &filename, const char* format=0);
};

#endif // RGB8_H
