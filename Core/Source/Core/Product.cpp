#include "Product.h"
#include <QDebug>

Product::Product(const QString& name, float currentPrice, bool available, const QString& url, 
				 const QStringList& imageUrls, std::optional<float> regularPrice)
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

QString Product::name() const
{
	return m_Name;
}

float Product::currentPrice() const
{
	return m_CurrentPrice;
}

bool Product::onSale() const
{
	return m_OnSale;
}

float Product::regularPrice() const
{
	return m_RegularPrice;
}

bool Product::available() const
{
	return m_Available;
}

QString Product::url() const
{
	return m_Url;
}

QStringList Product::imageUrls() const
{
	return m_ImageUrls;
}
