#include "Mapper.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <stdexcept>

Core::Product Core::mapToProduct(const QByteArray& fetchedContent)
{
    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(fetchedContent, &parseError);

    // Might need to refactor this, let's leave it like this for now
    if (parseError.error != QJsonParseError::NoError)
        throw std::runtime_error("JSON parse error");

    if (!doc.isObject())
        throw std::runtime_error("Expected JSON root object");

    // Basic product info
    const QJsonObject rootObj = doc.object();
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
                                     // isDoouble catches only true numbers
        const int regularPrice = compareAtPrice.toInt();
        return Product(name, currentPrice, available, url, imageUrls, regularPrice);
    }

    return Product(name, currentPrice, available, url, imageUrls);
}
