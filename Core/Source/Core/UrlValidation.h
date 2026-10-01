#pragma once
#include <QUrl>

bool isCompleteUrl(const QUrl& url)
{
    if (!url.isValid() || url.scheme().isEmpty() || url.host().isEmpty()) return false;
    else return true;
}
