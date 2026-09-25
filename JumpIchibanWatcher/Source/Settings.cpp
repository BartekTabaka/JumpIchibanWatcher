#include "Settings.h"

#include <array>
#include <expected>
#include <QDebug>
#include <QFile>
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

        // Debug variation (place config in working directory)
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

std::expected<Core::Product, SettingsError> Settings::loadLastProduct()
{
    const QSettings settings = MakeSettings();

    if (!QFile::exists(settings.fileName()))
        return std::unexpected(SettingsError{ SettingsErrorCode::FileNotFound, "Config file not found!" });

    switch (settings.status()) {
        case QSettings::NoError:
            break;
        case QSettings::FormatError:
            return std::unexpected(SettingsError{ SettingsErrorCode::InvalidFormat, "Config file is in wrong format!" });
        case QSettings::AccessError:
            return std::unexpected(SettingsError{ SettingsErrorCode::CannotOpenFile, "Couldn't open config file!" });
    }

    // Check if all the values exist and are proper type
    for (const auto& field : expectedValues) {
        const QString key = field.key.toString();
        const QString fullKey = FullKey(key);
        if (!settings.contains(fullKey))
            return std::unexpected(SettingsError{ SettingsErrorCode::MissingValue, QString("'%1' key not found in last product info!").arg(key)});

        // TODO: Validate value types (#1)
        //  - value.typeId() doesn't work
        // qDebug() << "Expected type:" << field.expectedType << "got:" << value.typeId();
    }
    
    const QString name          = settings.value(FullKey(u"name")).toString();
    const int currentPrice      = settings.value(FullKey(u"currentPrice")).toInt();
    const bool available        = settings.value(FullKey(u"available")).toBool();
    const QString url           = settings.value(FullKey(u"url")).toString();

    const QStringList imageUrls = settings.value(FullKey(u"imageUrls")).toStringList();
    if (imageUrls.isEmpty())
        return std::unexpected(SettingsError{ SettingsErrorCode::MissingValue, "'imageUrls' list is empty!" });

    const bool onSale = settings.value(FullKey(u"onSale")).toBool();
    if (onSale) {
        const int regularPrice = settings.value(FullKey(u"regularPrice")).toInt();
        return Core::Product(name, currentPrice, available, url, imageUrls, regularPrice);
    }

    return Core::Product(name, currentPrice, available, url, imageUrls);
}

std::expected<void, SettingsError> Settings::saveProduct(const Core::Product& product)
{
    QSettings settings = MakeSettings();

    if (!QFile::exists(settings.fileName()))
        qWarning() << "Settings file was not found! Creating new one!";

    switch (settings.status()) {
        case QSettings::NoError:
            break;
        case QSettings::FormatError:
            return std::unexpected(SettingsError{ SettingsErrorCode::InvalidFormat, "Config file is in wrong format!" });
        case QSettings::AccessError:
            return std::unexpected(SettingsError{ SettingsErrorCode::CannotOpenFile, "Couldn't open config file!" });
    }

    settings.beginGroup(kLastProductKey);
    settings.remove("");

    settings.setValue("name", product.name());
    settings.setValue("currentPrice", product.currentPrice());
    settings.setValue("onSale", product.onSale());
    settings.setValue("regularPrice", product.regularPrice());
    settings.setValue("available", product.available());
    settings.setValue("url", product.url());
    settings.setValue("imageUrls", product.imageUrls());

    settings.endGroup();
    settings.sync();

    switch (settings.status()) {
        case QSettings::FormatError:
            return std::unexpected(SettingsError{ SettingsErrorCode::InvalidFormat, "Config file is in wrong format!" });
        case QSettings::AccessError:
            return std::unexpected(SettingsError{ SettingsErrorCode::CannotOpenFile, "Couldn't open config file!" });
    }
    return {};

    //////////////////////////////////////////////////////////////////////////////////
    //                                                                              //
    //  We check status() twice because we are answering two different questions    //
    //  at two different points in time: the first switch (before writing) catches  //
    //  problems that already existed (e.g. a corrupted/inaccessible file found     //
    //  when opening it). The second switch (after sync()) catches problems with    //
    //  the actual write that just happened (e.g. running out of disk space,        //
    //  permissions being lost in the meantime) - the first check could not detect  //
    //  these because the write had not happened yet at that point.                 //
    //                                                                              //
    //////////////////////////////////////////////////////////////////////////////////
}
