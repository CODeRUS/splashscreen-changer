#pragma once
#include <QSize>
#include <QString>
#include <QDebug>

class ISplashscreen
{
public:
    virtual QString readSplashscreen() const = 0;
    virtual QString writeSplashscreen(const QString &fileName) const = 0;
    virtual QSize size() const = 0;
};
