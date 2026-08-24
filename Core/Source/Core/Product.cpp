#include "Product.h"

#include <QDebug>

Core::Product::Product(const QString& name, int currentPrice, bool available, const QString& url, 
				 const QStringList& imageUrls, std::optional<int> regularPrice)
	: m_Name(name), m_CurrentPrice(currentPrice), m_Available(available), m_Url(url),
	  m_ImageUrls(imageUrls)
{
	if (regularPrice) {
		m_OnSale = true;
		m_RegularPrice = regularPrice.value();
	}

	// This log block is just for now, I'll shorten it in the future to:
	// qDebug() << "Created new Product!";
	qDebug() << "Created new Product:";
	qDebug() << "- name:" << m_Name;
	qDebug() << "- current price:" << m_CurrentPrice;
	qDebug() << "- on sale:" << m_OnSale;
	if (regularPrice) qDebug() << "- regular price:" << m_RegularPrice;
	qDebug() << "- available:" << m_Available;
	qDebug() << "- url:" << m_Url;
	for (const QString& iUrl : m_ImageUrls)
		qDebug() << "- image:" << iUrl;
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

QString Core::Product::url() const
{
	return m_Url;
}

QStringList Core::Product::imageUrls() const
{
	return m_ImageUrls;
}
