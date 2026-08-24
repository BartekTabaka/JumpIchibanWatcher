#pragma once
#include "Product.h"
#include <QByteArray>

namespace Core
{
	Product mapToProduct(const QByteArray& fetchedContent);
}