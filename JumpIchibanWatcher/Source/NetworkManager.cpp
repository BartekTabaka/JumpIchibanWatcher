#include "NetworkManager.h"

#include <expected>
#include <QByteArray>
#include <QDebug>
#include <QEventLoop>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QString>
#include <QUrl>
#include <utility>

namespace
{
	constexpr int kTransferTimeout = 15000; // 15s

	NetworkError makeNetworkError(QNetworkReply *reply)
	{
		const int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
		const QString message = reply->errorString(); // code - HTTP code: x (MESSAGE)

		// No HTTP response
		switch (reply->error()) {
			case QNetworkReply::TimeoutError:
				return { NetworkErrorCode::Timeout, httpStatus, message };
			case QNetworkReply::OperationCanceledError:
				return { NetworkErrorCode::Cancelled, httpStatus, message };
			case QNetworkReply::SslHandshakeFailedError:
				return { NetworkErrorCode::SslError, httpStatus, message };
			case QNetworkReply::ConnectionRefusedError:
			case QNetworkReply::RemoteHostClosedError:
			case QNetworkReply::HostNotFoundError:
			case QNetworkReply::TemporaryNetworkFailureError:
			case QNetworkReply::NetworkSessionFailedError:
			case QNetworkReply::UnknownNetworkError:
				return { NetworkErrorCode::ConnectionError, httpStatus, message };
			default:
				break;
		}

		// HTTP status
		switch (httpStatus) {
			case 404:
				return { NetworkErrorCode::NotFound, httpStatus, message };
			case 429:
			case 503:
				return { NetworkErrorCode::RateLimited, httpStatus, message };
			default:
				break;
		}

		if (httpStatus != 0)
			return { NetworkErrorCode::HttpError, httpStatus, message };

		return { NetworkErrorCode::Unknown, httpStatus, message };
	}
} // namespace

NetworkManager::NetworkManager()
{
	m_NetworkManager.setTransferTimeout(kTransferTimeout);
}

std::expected<QByteArray, NetworkError> NetworkManager::fetchData(const QUrl& url)
{
	if (!url.isValid() || url.scheme().isEmpty() || url.host().isEmpty())
		return "Entered URL is invalid!"; // Later: std::unexpected

	// Request and headers
	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::UserAgentHeader,
		QByteArray("Mozilla/5.0 (Windows NT 10.0; Win64; x64) "
				   "AppleWebKit/537.36 (KHTML, like Gecko) "
				   "Chrome/140.0.0.0 Safari/537.36"));
	request.setRawHeader("Accept", "application/json, text/javascript, */*; q=0.01");

	// Reply
	QNetworkReply *reply = m_NetworkManager.get(request);
	m_PendingReply = reply;

	// Event loop
	QEventLoop eventLoop;
	QObject::connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
	eventLoop.exec();

	std::expected<QByteArray, NetworkError> result;
	const int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

	// Success
	if (reply->error() == QNetworkReply::NoError && httpStatus == 200) {
		result = reply->readAll();
	}
	// Error
	else {
		NetworkError error = makeNetworkError(reply);
		result = std::unexpected(std::move(error));
	}

	m_PendingReply.clear();
	reply->deleteLater();
	return result;
}

bool NetworkManager::isRequestPending() const
{
	return !m_PendingReply.isNull();
}
