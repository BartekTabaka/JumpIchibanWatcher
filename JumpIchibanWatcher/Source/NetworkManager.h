#pragma once
#include <QNetworkAccessManager>

#include <expected>
#include <QByteArray>
#include <QDebug>
#include <QNetworkReply>
#include <QObject>
#include <QPointer>
#include <QString>
#include <QUrl>

enum class NetworkErrorCode {
	ConnectionError,   // brak sieci, DNS, odmowa połączenia
	Timeout,
	SslError,
	NotFound,          // HTTP 404
	RateLimited,       // HTTP 429 / 503
	HttpError,         // inny status HTTP != 2xx
	Cancelled,         // OperationCanceledError (np. abort())
	Unknown
};

struct NetworkError {
	NetworkErrorCode code;
	int httpStatus = 0;
	QString message;

	bool isRetryable() const
	{
		switch (code) {
			case NetworkErrorCode::ConnectionError:
			case NetworkErrorCode::Timeout:
			case NetworkErrorCode::RateLimited:
				return true;
			default:
				return false;
		}
	}

	friend QDebug operator<<(QDebug debug, const NetworkError& error)
	{
		QDebugStateSaver saver(debug);

		// Error code
		debug.noquote() << "NetworkError:";
		switch (error.code) {
			case NetworkErrorCode::ConnectionError: debug << "connection error"; break;
			case NetworkErrorCode::Timeout:         debug << "timeout"; break;
			case NetworkErrorCode::SslError:        debug << "SSL error"; break;
			case NetworkErrorCode::NotFound:        debug << "not found"; break;
			case NetworkErrorCode::RateLimited:     debug << "rate limited"; break;
			case NetworkErrorCode::HttpError:       debug << "HTTP error"; break;
			case NetworkErrorCode::Cancelled:       debug << "cancelled"; break;
			default:                                debug << "unknown error"; break;
		}

		// HTTP status and message
		if (error.httpStatus != 0) {
			debug << "- HTTP code:" << error.httpStatus;
		}
		if (!error.message.isEmpty()) {
			debug << '(' + error.message + ')';
		}

		return debug;
	}
};

class NetworkManager : public QObject {
	Q_OBJECT
public:
	NetworkManager();

	std::expected<QByteArray, NetworkError> fetchData(const QUrl& url);

	bool isRequestPending() const;
	// TODO: can add some error handling functions like retry fetch, if rate limit has been met
private:
	QNetworkAccessManager m_NetworkManager;

	QPointer<QNetworkReply> m_PendingReply;
};
