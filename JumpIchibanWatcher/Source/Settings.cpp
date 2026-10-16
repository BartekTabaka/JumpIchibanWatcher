#include "Settings.h"

#include "AppIdentity.h"
#include "Core/Product.h"
#include "Core/UrlValidation.h"

#include <expected>
#include <QDebug>
#include <QFile>
#include <QList>
#include <QMetaType>
#include <QSettings>
#include <QString>
#include <QStringView>
#include <QUrl>
#include <QVariant>
#include <utility>

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
    const QString kLastProductKey = QStringLiteral("lastProduct");
    QString FullKey(QStringView field)
    {
        return QString(kLastProductKey + u'/' + field.toString());
    }

    // Alias for a typed result
    template<typename T>
    using Result = std::expected<T, SettingsError>;

    // std::unexpected construct helper
    SettingsError LoadError(SettingsErrorCode errorCode, QString message)
    {
        return { SettingsOperation::Loading, errorCode, std::move(message) };
    }

    // Check if the specified value exists
    Result<QVariant> ReadRaw(const QSettings& settings, QStringView field)
    {
        const QString full = FullKey(field);
        if (!settings.contains(full))
            return std::unexpected(LoadError(SettingsErrorCode::MissingValue, QStringLiteral("'%1' key not found in last product info!").arg(field)));

        return settings.value(full);
    }

    // String
    Result<QString> ReadString(const QSettings& settings, QStringView field)
    {
        auto raw = ReadRaw(settings, field);
        if (!raw)
            return std::unexpected(raw.error());

        const QString value = raw->toString();
        // String should only be valid if it's not empty
        if (value.isEmpty())
            return std::unexpected(LoadError(SettingsErrorCode::InvalidValueType,
                QStringLiteral("'%1' field is not a valid string! (empty string detected)").arg(field))
            );
        
        return value;
    }

    // Int
    Result<int> ReadInt(const QSettings& settings, QStringView field)
    {
        auto raw = ReadRaw(settings, field);
        if (!raw)
            return std::unexpected(raw.error());

        bool ok = false;
        const int value = raw->toString().toInt(&ok);
        if (!ok)
            return std::unexpected(LoadError(SettingsErrorCode::InvalidValueType, QStringLiteral("'%1' field is not a valid integer!").arg(field)));

        return value;
    }

    // Bool
    Result<bool> ReadBool(const QSettings& settings, QStringView field)
    {
        auto raw = ReadRaw(settings, field);
        if (!raw)
            return std::unexpected(raw.error());

        const QString value = raw->toString().trimmed().toLower();
        if (value == "true") return true;
        if (value == "false") return false;

        // If the value is not neither "true" or "false", then we count it as invalid
        return std::unexpected(LoadError(SettingsErrorCode::InvalidValueType, QStringLiteral("'%1' field is not a valid boolean!").arg(field)));
    }

    // Url
    Result<QUrl> ReadUrl(const QSettings& settings, QStringView field)
    {
        auto raw = ReadRaw(settings, field);
        if (!raw)
            return std::unexpected(raw.error());
        
        const QUrl value(raw->toString());
        if (!isCompleteUrl(value))
            return std::unexpected(LoadError(SettingsErrorCode::InvalidValue,
                QStringLiteral("specified '%1' is invalid: \"%2\"").arg(field).arg(value.toString()))
            );

        return value;
    }

    // Url list
    Result<QList<QUrl>> ReadUrlList(const QSettings& settings, QStringView field)
    {
        auto raw = ReadRaw(settings, field);
        if (!raw)
            return std::unexpected(raw.error());

        const QStringList value(raw->toStringList());
        if (value.isEmpty())
            return std::unexpected(LoadError(SettingsErrorCode::MissingValue, QStringLiteral("'%1' list is empty!").arg(field)));

        const QList<QUrl> converted = QUrl::fromStringList(value);
        for (const QUrl& url : converted) {
            if (!isCompleteUrl(url))
                return std::unexpected(LoadError(SettingsErrorCode::InvalidValue, 
                    QStringLiteral("specified URL on '%1' list is invalid: \"%2\"").arg(field).arg(url.toString()))
                );
        }

        return converted;
    }
} // namespace

std::expected<Core::Product, SettingsError> Settings::loadLastProduct()
{
    const QSettings settings = MakeSettings();
    constexpr SettingsOperation operation = SettingsOperation::Loading;

    if (!QFile::exists(settings.fileName()))
        return std::unexpected(SettingsError{ operation, SettingsErrorCode::FileNotFound, QStringLiteral("Config file not found!") });

    switch (settings.status()) {
        case QSettings::NoError:
            break;
        case QSettings::FormatError:
            return std::unexpected(SettingsError{ operation, SettingsErrorCode::InvalidFormat, QStringLiteral("Config file is in wrong format!") });
        case QSettings::AccessError:
            return std::unexpected(SettingsError{ operation, SettingsErrorCode::CannotOpenFile, QStringLiteral("Couldn't open config file!") });
    }
    
    // -- Read variables ------------
    const auto name = ReadString(settings, u"name");
    if (!name) return std::unexpected(name.error());

    const auto currentPrice = ReadInt(settings, u"currentPrice");
    if (!currentPrice) return std::unexpected(currentPrice.error());

    const auto available = ReadBool(settings, u"available");
    if (!available) return std::unexpected(available.error());

    const auto url = ReadUrl(settings, u"url");
    if (!url) return std::unexpected(url.error());

    const auto featuredImageUrl = ReadUrl(settings, u"featuredImageUrl");
    if (!featuredImageUrl) return std::unexpected(featuredImageUrl.error());

    const auto imageUrls = ReadUrlList(settings, u"imageUrls");
    if (!imageUrls) return std::unexpected(imageUrls.error());

    // -- Check for sale ------------
    const auto onSale = ReadBool(settings, u"onSale");
    // Check if the boolean has bean loaded successfully
    if (!onSale) return std::unexpected(onSale.error());
    // If yes, we check the boolean value (indicating whether the product was on sale or not)
    if (*onSale) {
        const auto regularPrice = ReadInt(settings, u"regularPrice");
        if (!regularPrice) return std::unexpected(regularPrice.error());

        // Warn if the product is marked as on sale, but no price difference was detected
        if (*currentPrice == *regularPrice)
            qWarning() << "Product is marked as on sale, but currentPrice and regularPrice are identical!";

        return Core::Product(*name, *currentPrice, *available, *url, *imageUrls, *featuredImageUrl, *regularPrice);
    }

    return Core::Product(*name, *currentPrice, *available, *url, *imageUrls, *featuredImageUrl);
}

std::expected<void, SettingsError> Settings::saveProduct(const Core::Product& product)
{
    QSettings settings = MakeSettings();
    constexpr SettingsOperation operation = SettingsOperation::Saving;

    if (!QFile::exists(settings.fileName()))
        qWarning() << "Settings file was not found! Creating new one!";

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

    switch (settings.status()) {
        case QSettings::NoError:
            break;
        case QSettings::FormatError:
            return std::unexpected(SettingsError{ operation, SettingsErrorCode::InvalidFormat, QStringLiteral("Config file is in wrong format!") });
        case QSettings::AccessError:
            return std::unexpected(SettingsError{ operation, SettingsErrorCode::CannotOpenFile, QStringLiteral("Couldn't open config file!") });
    }

    settings.beginGroup(kLastProductKey);
    settings.remove("");

    settings.setValue("name", product.name());
    settings.setValue("currentPrice", product.currentPrice());
    settings.setValue("onSale", product.onSale());
    settings.setValue("regularPrice", product.regularPrice());
    settings.setValue("available", product.available());
    settings.setValue("url", product.url().toString());
    settings.setValue("imageUrls", QUrl::toStringList(product.imageUrls()));
    settings.setValue("featuredImageUrl", product.featuredImageUrl().toString());

    settings.endGroup();
    settings.sync();

    switch (settings.status()) {
        case QSettings::NoError:
            break;
        case QSettings::FormatError:
            return std::unexpected(SettingsError{ operation, SettingsErrorCode::InvalidFormat, QStringLiteral("Config file is in wrong format!") });
        case QSettings::AccessError:
            return std::unexpected(SettingsError{ operation, SettingsErrorCode::CannotOpenFile, QStringLiteral("Couldn't open config file!") });
    }

    qDebug() << "Product saved successfully!";
    qDebug() << "-----------------------";
    return {};
}
