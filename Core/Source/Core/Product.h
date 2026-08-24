#pragma once
#include <QString>
#include <QStringList>
#include <optional>

class Product {
public:
	Product(const QString& name, float currentPrice, bool available, const QString& url, 
			const QStringList& imageUrls, std::optional<float> regularPrice = std::nullopt);

	// Getters
	QString name() const;
	float currentPrice() const;
	bool onSale() const;
	float regularPrice() const;
	bool available() const;
	QString url() const;
	QStringList imageUrls() const;
private:
	QString m_Name;
	float m_CurrentPrice;
	bool m_OnSale = false;
	float m_RegularPrice = 0.f;
	bool m_Available;
	QString m_Url;
	QStringList m_ImageUrls;
};
