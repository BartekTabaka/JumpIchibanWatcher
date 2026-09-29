#pragma once
#include <QNetworkAccessManager>

#include <expected>
#include <QByteArray>
#include <QObject>
#include <QPointer>
#include <QString>
#include <QUrl>

class NetworkManager : public QObject {
	Q_OBJECT
public:
	NetworkManager();

	std::expected<QByteArray, QString> fetchProductJson(const QUrl& url);

	bool isRequestPending() const;
	// TODO: can add some error handling functions like retry fetch, if rate limit has been met
private:
	QNetworkAccessManager m_NetworkManager;

	QPointer<QNetworkReply> m_PendingReply;
};
