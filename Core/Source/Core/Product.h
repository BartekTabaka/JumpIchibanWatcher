#pragma once
#include <QString>
#include <QStringList>
#include <optional>

namespace Core
{
	class Product {
	public:
		Product(const QString& name, int currentPrice, bool available, const QString& url,
			const QStringList& imageUrls, std::optional<int> regularPrice = std::nullopt);

		// Getters
		QString name() const;
		int currentPrice() const;
		bool onSale() const;
		int regularPrice() const;
		bool available() const;
		QString url() const;
		QStringList imageUrls() const;
	private:
		QString m_Name;
		int m_CurrentPrice; // in cents
		bool m_OnSale = false;
		int m_RegularPrice; // in cents
		bool m_Available;
		QString m_Url;
		QStringList m_ImageUrls;
	};
}
