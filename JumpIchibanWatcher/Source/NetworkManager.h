#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <expected>

class NetworkManager : public QObject {
	Q_OBJECT
public:
	NetworkManager();

	std::expected<QByteArray, QString> fetchProductJson(const QUrl& url);

	// TODO: can add some error handling functions like retry fetch, if rate limit has been met
private:
	QNetworkAccessManager m_NetworkManager;
};
