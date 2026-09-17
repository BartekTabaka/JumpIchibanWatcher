#include "Settings.h"


#include <array>
#include <optional>
#include <QDebug>
#include <QMetaType>
#include <QSettings>
#include <QString>
#include <QStringView>
#include "AppIdentity.h"
#include "Core/Product.h"

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
    QString FullKey(QStringView field)
    {
        return QString(kLastProductKey + u'/' + field.toString());
    }

    struct ProductField {
        QStringView key;
        QMetaType::Type expectedType;
    };

    constexpr std::array<ProductField, 7> expectedValues{ {
        { u"name",         QMetaType::QString },
        { u"currentPrice", QMetaType::Int },
        { u"onSale",       QMetaType::Bool },
        { u"regularPrice", QMetaType::Int },
        { u"available",    QMetaType::Bool },
        { u"url",          QMetaType::QString },
        { u"imageUrls",    QMetaType::QStringList }
    } };
} // namespace

std::optional<Core::Product> Settings::LoadLastProduct()
{
    const QSettings settings = MakeSettings();

    // Check if all the values exist and are proper type
    for (const auto& field : expectedValues) {
        const QString fullKey = FullKey(field.key.toString());
        if (!settings.contains(fullKey))
            return std::nullopt;
        
        // TODO: Validate value types (#1)
        //  - value.typeId() doesn't work
        // qDebug() << "Expected type:" << field.expectedType << "got:" << value.typeId();*/
    }
    
    const QString name          = settings.value(FullKey(u"name")).toString();
    const int currentPrice      = settings.value(FullKey(u"currentPrice")).toInt();
    const bool available        = settings.value(FullKey(u"available")).toBool();
    const QString url           = settings.value(FullKey(u"url")).toString();

    const QStringList imageUrls = settings.value(FullKey(u"imageUrls")).toStringList();
    if (imageUrls.isEmpty())
        return std::nullopt;
    
    const bool onSale = settings.value(FullKey(u"onSale")).toBool();
    if (onSale) {
        const int regularPrice = settings.value(FullKey(u"regularPrice")).toInt();
        return Core::Product(name, currentPrice, available, url, imageUrls, regularPrice);
    }

    return Core::Product(name, currentPrice, available, url, imageUrls);
}

void Settings::SaveNewProduct(const Core::Product& product)
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
