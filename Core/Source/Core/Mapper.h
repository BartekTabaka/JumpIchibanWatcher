#pragma once
#include "Product.h"
#include <QByteArray>
#include <expected>
#include <QDebug>

namespace Core
{
    // Error code enum
    enum class JsonErrorCode {
        ParseError,
        RootObjectNotFound,
        KeyNotFound,
        InvalidValueType
    };

    // Structured error with context
    struct JsonError {
        JsonErrorCode code;
        QString message;

        friend QDebug operator<<(QDebug debug, JsonError error)
        {
            debug << "JsonError:";
            switch (error.code) {
            case JsonErrorCode::ParseError:         debug << "parsing error"; break;
            case JsonErrorCode::RootObjectNotFound: debug << "root object not found"; break;
            case JsonErrorCode::KeyNotFound:        debug << "key not found"; break;
            }
            if (!error.message.isEmpty()) {
                QDebugStateSaver saver(debug);
                debug.noquote() << '(' + error.message + ')';
            }

            return debug;
        }
    };

	std::expected<Product, JsonError> mapToProduct(const QByteArray& fetchedContent);
}