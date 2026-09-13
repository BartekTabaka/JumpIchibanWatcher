#include "NetworkManager.h"

#include <QByteArray>
#include <QDebug>
#include <QEventLoop>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QString>
#include <QUrl>

NetworkManager::NetworkManager()
{
	m_NetworkManager.setTransferTimeout(15000); // 15s
}

std::expected<QByteArray, QString> NetworkManager::fetchProductJson(const QUrl& url)
{
	// Modify URL
	QUrl targetUrl = url;
	targetUrl.setPath(targetUrl.path() + ".js");
	qDebug() << "Modified URL:" << targetUrl.toString();
	qDebug() << "-----------------------";

	// Request and headers
	QNetworkRequest request(targetUrl);
	request.setHeader(QNetworkRequest::UserAgentHeader,
		QByteArray("Mozilla/5.0 (Windows NT 10.0; Win64; x64) "
			"AppleWebKit/537.36 (KHTML, like Gecko) "
			"Chrome/140.0.0.0 Safari/537.36"));
	request.setRawHeader("Accept", "application/json, text/javascript, */*; q=0.01");

	// Reply
	QNetworkReply *reply = m_NetworkManager.get(request);

	// Event loop
	QEventLoop eventLoop;
	QObject::connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
	eventLoop.exec();

	const int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	// Success
	if (reply->error() == QNetworkReply::NoError && httpStatus == 200) {
		const QByteArray data = reply->readAll();
		reply->deleteLater();
		return data;
	}
	// Errors
	else if (httpStatus == 404) {
		qWarning() << "Product doesn't exist";
	}
	else if (httpStatus == 429 || httpStatus == 503) {
		qWarning() << "Rate limit, code:" << httpStatus << "- slow down!";
	}
	else {
		qWarning() << "Something went wrong:" << reply->errorString() << "HTTP" << httpStatus;
	}

	reply->deleteLater();
	return std::unexpected(reply->errorString());
}
