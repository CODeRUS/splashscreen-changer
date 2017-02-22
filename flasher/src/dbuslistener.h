#ifndef DBUSLISTENERFLASHLIGHT_H
#define DBUSLISTENERFLASHLIGHT_H

#include <QObject>

#include <QtDBus>

class DBusListener : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.coderus.splashscreen.flasher")
public:
    explicit DBusListener(QObject *parent = 0);

public slots:
    Q_SCRIPTABLE void flashOriginal();
    Q_SCRIPTABLE void flashPicture(const QString &fileName);
    Q_SCRIPTABLE void readPicture();
    Q_SCRIPTABLE void quit();

public slots:
    void startService();

private:
    void flashImage(const QByteArray &image);
    void errorMessage(const QString &message);

    bool gzipDecompress(QByteArray input, QByteArray &output);

signals:
    Q_SCRIPTABLE void pictureRead(const QString &filename);
    Q_SCRIPTABLE void someError(const QString &errorMessage);

};

#endif // DBUSLISTENERFLASHLIGHT_H
