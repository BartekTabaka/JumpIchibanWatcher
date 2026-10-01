#include "Mapper.h"

#include <array>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QList>
#include <QStringView>
#include <QUrl>
#include "Product.h"
#include "UrlValidation.h"

namespace
{
    struct JsonField {
        QStringView key;
        QJsonValue::Type expectedType;
    };
    
    constexpr std::array<JsonField, 6> expectedFields { {
        { u"title",          QJsonValue::String },
        { u"price",          QJsonValue::Double },
        { u"available",      QJsonValue::Bool },
        { u"url",            QJsonValue::String },
        { u"images",         QJsonValue::Array },
        { u"featured_image", QJsonValue::String }
    } };

    QString constructInvalidUrlErrorMessage(const QUrl& url)
    {
        QString message;
        if (url.isEmpty()) message = "specified URL is empty!";
        else               message = QString("specified URL is invalid! Error: %1").arg(url.errorString());

        return message;
    }
} // namespace

namespace Core
{
    std::expected<Product, JsonError> mapToProduct(const QByteArray& fetchedContent, const QUrl& baseUrl)
    {
        QJsonParseError parseError;
        const QJsonDocument doc = QJsonDocument::fromJson(fetchedContent, &parseError);

        // Check for parsing error
        if (parseError.error != QJsonParseError::NoError)
            return std::unexpected(JsonError{ JsonErrorCode::ParseError, parseError.errorString() });

        // Check if there's a root object
        if (!doc.isObject())
            return std::unexpected(JsonError{ JsonErrorCode::RootObjectNotFound, "no root object found!" });

        // -- Basic product info --------------------
        const QJsonObject rootObj = doc.object();

        for (const auto& field : expectedFields) {
            const auto key = field.key.toString();

            // Check if all of the necessary values exist
            if (!rootObj.contains(key))
                return std::unexpected(JsonError{ 
                    JsonErrorCode::KeyNotFound, 
                    QString("'%1' key not found!").arg(key) 
                });

            // Check values types
            const auto value = rootObj.value(key);
            if (value.type() != field.expectedType)
                return std::unexpected(JsonError{ 
                    JsonErrorCode::InvalidValueType,
                    QString("'%1' key value has invalid type!").arg(key)
                });
        }
        // compare_at_price is optional in terms of type (double or null),
        // so it is validated separately from the fields with a single expected type.
        if (!rootObj.contains("compare_at_price"))
            return std::unexpected(JsonError{ JsonErrorCode::KeyNotFound, "'compare_at_price' key not found!" });

        const QString name = rootObj.value("title").toString();
        const int currentPrice = rootObj.value("price").toInt();
        const bool available = rootObj.value("available").toBool();
        const QUrl rawUrl = QUrl(rootObj.value("url").toString());
        const QJsonArray imagesUrlsArray = rootObj.value("images").toArray();
        const QUrl rawFeaturedImageUrl = QUrl(rootObj.value("featured_image").toString());

        // Validate URLs
        if (!rawUrl.isValid())
            return std::unexpected(JsonError{ JsonErrorCode::InvalidValue, constructInvalidUrlErrorMessage(rawUrl) });
        const QUrl fullUrl = baseUrl.resolved(rawUrl);
        if (!isCompleteUrl(fullUrl))
            return std::unexpected(JsonError{ JsonErrorCode::InvalidValue, constructInvalidUrlErrorMessage(fullUrl) });
        
        if (!rawFeaturedImageUrl.isValid())
            return std::unexpected(JsonError{ JsonErrorCode::InvalidValue, constructInvalidUrlErrorMessage(rawFeaturedImageUrl) });
        const QUrl fullFeaturedImageUrl = baseUrl.resolved(rawFeaturedImageUrl);
        if (!isCompleteUrl(fullFeaturedImageUrl))
            return std::unexpected(JsonError{ JsonErrorCode::InvalidValue, constructInvalidUrlErrorMessage(fullFeaturedImageUrl) });

        QList<QUrl> imageUrls;
        imageUrls.reserve(imagesUrlsArray.size());
        for (const QJsonValue& value : imagesUrlsArray) {
            if (!value.isString())
                return std::unexpected(JsonError{
                    JsonErrorCode::InvalidValueType,
                    "one of imagesUrlsArray values has invalid type!"
                });

            const QUrl rawImageUrl = QUrl(value.toString());
            if (!rawImageUrl.isValid())
                return std::unexpected(JsonError{ JsonErrorCode::InvalidValue, constructInvalidUrlErrorMessage(rawImageUrl) });

            imageUrls.append(baseUrl.resolved(rawImageUrl));
        }

        qDebug() << "Parsed product from JSON";
        qDebug() << "-----------------------";

        // Check if the product is on sale
        const QJsonValue compareAtPrice = rootObj.value("compare_at_price");
        if (compareAtPrice.isDouble()) { // isNull would return also if the key doesn't exist
                                         // isDouble catches only true numbers
            const int regularPrice = compareAtPrice.toInt();
            return Product(name, currentPrice, available, fullUrl, imageUrls, fullFeaturedImageUrl, regularPrice);
        }

        return Product(name, currentPrice, available, fullUrl, imageUrls, fullFeaturedImageUrl);
    }
} // namespace Core
