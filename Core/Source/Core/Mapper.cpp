#include "Mapper.h"

#include <array>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>

namespace
{
    struct JsonField {
        QStringView key;
        QJsonValue::Type expectedType;
    };

    constexpr std::array<JsonField, 5> expectedFields{ {
        { u"title",     QJsonValue::String },
        { u"price",     QJsonValue::Double },
        { u"available", QJsonValue::Bool },
        { u"url",       QJsonValue::String },
        { u"images",    QJsonValue::Array },
    } };
}

namespace Core
{
    std::expected<Product, JsonError> mapToProduct(const QByteArray& fetchedContent)
    {
        QJsonParseError parseError;
        const QJsonDocument doc = QJsonDocument::fromJson(fetchedContent, &parseError);

        // Check for parsing error
        if (parseError.error != QJsonParseError::NoError)
            return std::unexpected(JsonError{ JsonErrorCode::ParseError, parseError.errorString() });

        // Check if there's a root object
        if (!doc.isObject())
            return std::unexpected(JsonError{ JsonErrorCode::RootObjectNotFound, "No root object found" });

        // -- Basic product info --------------------
        const QJsonObject rootObj = doc.object();

        for (const auto& field : expectedFields) {
            // Check if all of the necessary values exist
            if (!rootObj.contains(field.key))
                return std::unexpected(JsonError{ 
                    JsonErrorCode::KeyNotFound, 
                    QString("'%1' key not found").arg(field.key) 
                });

            // Check values types
            const auto value = rootObj.value(field.key);
            if (value.type() != field.expectedType)
                return std::unexpected(JsonError{ 
                    JsonErrorCode::InvalidValueType,
                    QString("'%1' key value has invalid type").arg(field.key)
                });
        }
        // compare_at_price is optional in terms of type (double or null),
        // so it is validated separately from the fields with a single expected type.
        if (!rootObj.contains("compare_at_price"))
            return std::unexpected(JsonError{ JsonErrorCode::KeyNotFound, "'compare_at_price' key not found" });

        /*// Check if all of the necessary values exist
        if (!rootObj.contains("title"))
            return std::unexpected(JsonError{ JsonErrorCode::KeyNotFound, "'title' key not found" });
        if (!rootObj.contains("price"))
            return std::unexpected(JsonError{ JsonErrorCode::KeyNotFound, "'price' key not found" });
        if (!rootObj.contains("available"))
            return std::unexpected(JsonError{ JsonErrorCode::KeyNotFound, "'available' key not found" });
        if (!rootObj.contains("url"))
            return std::unexpected(JsonError{ JsonErrorCode::KeyNotFound, "'url' key not found" });
        if (!rootObj.contains("images"))
            return std::unexpected(JsonError{ JsonErrorCode::KeyNotFound, "'images' key not found" });
        if (!rootObj.contains("compare_at_price"))
            return std::unexpected(JsonError{ JsonErrorCode::KeyNotFound, "'compare_at_price' key not found" });

        // Check values types
        if (!rootObj.value("title").isString())
            return std::unexpected(JsonError{ JsonErrorCode::InvalidValueType, "'title' key value isn't string" });
        if (!rootObj.value("price").isDouble())
            return std::unexpected(JsonError{ JsonErrorCode::InvalidValueType, "'price' key value isn't double" });
        if (!rootObj.value("available").isBool())
            return std::unexpected(JsonError{ JsonErrorCode::InvalidValueType, "'available' key value isn't bool" });
        if (!rootObj.value("url").isString())
            return std::unexpected(JsonError{ JsonErrorCode::InvalidValueType, "'url' key value isn't string" });
        if (!rootObj.value("images").isArray())
            return std::unexpected(JsonError{ JsonErrorCode::InvalidValueType, "'images' key value isn't array" });*/

        const QString name = rootObj.value("title").toString();
        const int currentPrice = rootObj.value("price").toInt();
        const bool available = rootObj.value("available").toBool();
        const QString url = rootObj.value("url").toString();

        // Images URLs
        const QJsonArray imagesUrlsArray = rootObj.value("images").toArray();
        QStringList imageUrls;
        imageUrls.reserve(imagesUrlsArray.size());
        for (const QJsonValue& value : imagesUrlsArray) {
            if (value.isString())
                imageUrls.append(value.toString());
        }

        // Check if the product is on sale
        const QJsonValue compareAtPrice = rootObj.value("compare_at_price");
        if (compareAtPrice.isDouble()) { // isNull would return also if the key doesn't exist
                                         // isDouble catches only true numbers
            const int regularPrice = compareAtPrice.toInt();
            return Product(name, currentPrice, available, url, imageUrls, regularPrice);
        }

        return Product(name, currentPrice, available, url, imageUrls);
    }
}
