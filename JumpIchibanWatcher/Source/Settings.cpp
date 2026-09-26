#include "Settings.h"

#include <array>
#include <expected>
#include <QDebug>
#include <QFile>
#include <QList>
#include <QMetaType>
#include <QSettings>
#include <QString>
#include <QStringView>
#include <QUrl>
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
    constexpr SettingsOperation operation = SettingsOperation::Loading;

    if (!QFile::exists(settings.fileName()))
        return std::unexpected(SettingsError{ operation, SettingsErrorCode::FileNotFound, "Config file not found!" });

    switch (settings.status()) {
        case QSettings::NoError:
            break;
        case QSettings::FormatError:
            return std::unexpected(SettingsError{ operation, SettingsErrorCode::InvalidFormat, "Config file is in wrong format!" });
        case QSettings::AccessError:
            return std::unexpected(SettingsError{ operation, SettingsErrorCode::CannotOpenFile, "Couldn't open config file!" });
    }

    // Check if all the values exist and are proper type
    for (const auto& field : expectedValues) {
        const QString key = field.key.toString();
        const QString fullKey = FullKey(key);
        if (!settings.contains(fullKey))
            return std::unexpected(SettingsError{ operation, SettingsErrorCode::MissingValue, QString("'%1' key not found in last product info!").arg(key)});

        // TODO: Validate value types (#1)
        //  - value.typeId() doesn't work
        // qDebug() << "Expected type:" << field.expectedType << "got:" << value.typeId();
    }
    
    // -- Read variables ------------
    const QString name          = settings.value(FullKey(u"name")).toString();
    const int currentPrice      = settings.value(FullKey(u"currentPrice")).toInt();
    const bool available        = settings.value(FullKey(u"available")).toBool();
    const QString urlString     = settings.value(FullKey(u"url")).toString();

    const QStringList imageUrlsStrings = settings.value(FullKey(u"imageUrls")).toStringList();
    if (imageUrlsStrings.isEmpty())
        return std::unexpected(SettingsError{ operation, SettingsErrorCode::MissingValue, "'imageUrls' list is empty!" });

    // Validate URLs
    const QUrl urlConverted = QUrl(urlString);
    if (!urlConverted.isValid())
        return std::unexpected(SettingsError{ operation, SettingsErrorCode::InvalidValue, "specified URL is invalid!" });
    // -- FOR DEBUGGING ------------
    else
        qDebug() << urlConverted.toString();

    const QList<QUrl> imageUrlsConverted = QUrl::fromStringList(imageUrlsStrings);
    for (const QUrl& url : imageUrlsConverted) {
        if (!url.isValid())
            return std::unexpected(SettingsError{ operation, SettingsErrorCode::InvalidValue, QString("specified URL on image list is invalid:\n%1").arg(url.toString()) });
    }

    // Check for sale
    const bool onSale = settings.value(FullKey(u"onSale")).toBool();
    if (onSale) {
        const int regularPrice = settings.value(FullKey(u"regularPrice")).toInt();
        return Core::Product(name, currentPrice, available, urlConverted, imageUrlsConverted, regularPrice);
    }

    return Core::Product(name, currentPrice, available, urlConverted, imageUrlsConverted);
}

std::expected<void, SettingsError> Settings::saveProduct(const Core::Product& product)
{
    QSettings settings = MakeSettings();
    constexpr SettingsOperation operation = SettingsOperation::Saving;

    if (!QFile::exists(settings.fileName()))
        qWarning() << "Settings file was not found! Creating new one!";

    switch (settings.status()) {
        case QSettings::NoError:
            break;
        case QSettings::FormatError:
            return std::unexpected(SettingsError{ operation, SettingsErrorCode::InvalidFormat, "Config file is in wrong format!" });
        case QSettings::AccessError:
            return std::unexpected(SettingsError{ operation, SettingsErrorCode::CannotOpenFile, "Couldn't open config file!" });
    }

    settings.beginGroup(kLastProductKey);
    settings.remove("");

    settings.setValue("name", product.name());
    settings.setValue("currentPrice", product.currentPrice());
    settings.setValue("onSale", product.onSale());
    settings.setValue("regularPrice", product.regularPrice());
    settings.setValue("available", product.available());
    settings.setValue("url", product.url());
    settings.setValue("imageUrls", QUrl::toStringList(product.imageUrls()));

    settings.endGroup();
    settings.sync();

    switch (settings.status()) {
        case QSettings::FormatError:
            return std::unexpected(SettingsError{ operation, SettingsErrorCode::InvalidFormat, "Config file is in wrong format!" });
        case QSettings::AccessError:
            return std::unexpected(SettingsError{ operation, SettingsErrorCode::CannotOpenFile, "Couldn't open config file!" });
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
