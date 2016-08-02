#ifndef RGB8_H
#define RGB8_H

#include <QObject>
#include <QImage>

class RGB8 : public QObject
{
    Q_OBJECT
public:
    explicit RGB8(QObject *parent = 0);

    Q_INVOKABLE void toRawData(const QString &filename);

signals:

public slots:
};

#endif // RGB8_H
