#include "Product.h"

#include <optional>
#include <QDebug>

Core::Product::Product(const QString& name, int currentPrice, bool available, const QUrl& url, 
				 const QList<QUrl>& imageUrls, const QUrl& featuredImageUrl, std::optional<int> regularPrice)
	: m_Name(name), m_CurrentPrice(currentPrice), m_Available(available), m_Url(url),
	  m_ImageUrls(imageUrls), m_FeaturedImageUrl(featuredImageUrl)
{
	if (regularPrice) {
		m_OnSale = true;
		m_RegularPrice = regularPrice.value();
	}
	else m_RegularPrice = m_CurrentPrice;
	// If the product is not on sale, assume that the regular price is the same as the current price
}

QString Core::Product::name() const
{
	return m_Name;
}

int Core::Product::currentPrice() const
{
	return m_CurrentPrice;
}

bool Core::Product::onSale() const
{
	return m_OnSale;
}

int Core::Product::regularPrice() const
{
	return m_RegularPrice;
}

bool Core::Product::available() const
{
	return m_Available;
}

QUrl Core::Product::url() const
{
	return m_Url;
}

QList<QUrl> Core::Product::imageUrls() const
{
	return m_ImageUrls;
}

QUrl Core::Product::featuredImageUrl() const
{
	return m_FeaturedImageUrl;
}
