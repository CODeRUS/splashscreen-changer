#pragma once
#include "ISplashscreen.hpp"

class T5Splashscreen : public ISplashscreen
{
public:
    T5Splashscreen();

    QString readSplashscreen() const override;
    QString writeSplashscreen(const QString &fileName) const override;
    QSize size() const override;
};

