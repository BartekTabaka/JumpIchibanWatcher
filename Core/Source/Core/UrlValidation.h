#pragma once
#include <QUrl>

inline bool isCompleteUrl(const QUrl& url)
{
    return url.isValid() || !url.scheme().isEmpty() || !url.host().isEmpty();
}
