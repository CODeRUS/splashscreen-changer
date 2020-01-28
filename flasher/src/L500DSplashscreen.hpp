#pragma once
#include "ISplashscreen.hpp"

class L500DSplashscreen : public ISplashscreen
{
public:
    L500DSplashscreen();

    QString readSplashscreen() const override;
    QString writeSplashscreen(const QString &fileName) const override;
    QSize size() const override;
};

