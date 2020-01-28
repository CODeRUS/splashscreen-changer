#pragma once
#include "ISplashscreen.hpp"

class TBJSplashscreen : public ISplashscreen
{
public:
    TBJSplashscreen();

    // ISplashscreen interface
public:
    QString readSplashscreen() const override;
    QString writeSplashscreen(const QString &fileName) const override;
    QSize size() const override;
};

