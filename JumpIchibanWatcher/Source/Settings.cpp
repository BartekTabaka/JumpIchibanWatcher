#include "Settings.h"

#include "AppIdentity.h"
#include <QDebug>

namespace
{
    QSettings MakeSettings()
    {
        // Release variation
        //return QSettings(QSettings::IniFormat, QSettings::UserScope, AppIdentity::kOrganizationName, AppIdentity::kApplicationName);

        // Debug variation (place config next to .exe)
        return QSettings("config.ini", QSettings::IniFormat);
    }

    // If we add more variables for the keys, this will need to become a namespace.
    const QString kLastProductKey = "lastProduct";
}

Core::Product Settings::LoadLastProduct()
{
    const QSettings settings = MakeSettings();

    const QString name = settings.value(kLastProductKey + "/name", "unknown").toString();
    const int currentPrice = settings.value(kLastProductKey + "/currentPrice", 0).toInt();
    const bool available = settings.value(kLastProductKey + "/available", false).toBool();
    const QString url = settings.value(kLastProductKey + "/url", "").toString();
    const QStringList imageUrls = settings.value(kLastProductKey + "/imageUrls").toStringList();

    const bool onSale = settings.value(kLastProductKey + "/onSale", false).toBool();
    if (onSale) {
        const int regularPrice = settings.value(kLastProductKey + "/regularPrice", 0).toInt();
        return Core::Product(name, currentPrice, available, url, imageUrls, regularPrice);
    }

    return Core::Product(name, currentPrice, available, url, imageUrls);
}

void Settings::SaveNewProduct(Core::Product product)
{
    QSettings settings = MakeSettings();

    settings.beginGroup(kLastProductKey);

    settings.setValue("name", product.name());
    settings.setValue("currentPrice", product.currentPrice());
    settings.setValue("onSale", product.onSale());
    settings.setValue("regularPrice", product.regularPrice());
    settings.setValue("available", product.available());
    settings.setValue("url", product.url());
    settings.setValue("imageUrls", product.imageUrls());

    settings.endGroup();
}
