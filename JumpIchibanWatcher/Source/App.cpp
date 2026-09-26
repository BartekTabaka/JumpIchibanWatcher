#include "App.h"

#include <optional>
#include <QApplication>
#include <QDebug>
#include <QString>
#include <QUrl>
#include <utility>
#include "Core/Product.h"
#include "Settings.h"

namespace
{
	// TOOD: add currencies

	// Price wrapper
	QString price(int cents)
	{
		double full = (static_cast<double>(cents) / 100);
		return QString::number(full, 'f', 2) + " PLN";
	}

	// Logging helper
	void logPrice(const QString& label, const QString& value)
	{
		QDebug dbg = qDebug();
		QDebugStateSaver saver(dbg);
		dbg.noquote();

		dbg << label << value;
	}

	// Helper for now
	QUrl getUrl()
	{
		QUrl url;
		url.setScheme("https");
		url.setHost("jumpichiban.com");
		url.setPath("/products/ichiban-kuji-my-dress-up-darling-season-2-last-one-prize-marin-kitagawa-artscale-memoria-figure");
		// The user will simply copy and paste the url of HTML site, '.js' will be added inside the NetworkManager class

		return url;
	}

	std::optional<Core::Product> loadLastProductOrLog()
	{
		auto result = Settings::loadLastProduct();
		if (!result) {
			qCritical() << result.error();
			return std::nullopt;
		}

		return std::move(*result);
	}
} // namespace

App *g_App = nullptr;

App::App(QApplication& app) : m_App(app),
							  m_LastProduct(loadLastProductOrLog())
{
	if (!m_LastProduct)
		qWarning() << "Loading last product info from settings failed!";
	else
		qDebug() << "Loaded last product info from settings";

	qDebug() << "The app is working!";
	qDebug() << "-----------------------";
}

void App::refreshProduct()
{
	////////////////////////////////////////////////////////////////////////////////////
	//																				  //
	//  NOTE: Fetch and parsing return different error types (FetchError vs           //
	//  JsonError), so `.and_then()` can't chain them directly - it would require     //
	//  `.transform_error()` first to unify both into one type. Plain `if`s are more  //
	//  readable here, so I'm sticking with them on purpose, not as a workaround.     //
	//																				  //
	////////////////////////////////////////////////////////////////////////////////////

	auto fetchedContent = m_NetworkManager.fetchProductJson(getUrl());
	if (!fetchedContent) {
		qCritical() << "Fetching error:" << fetchedContent.error();
		return;
	}

	// NOTE: fetchedContent.value() == *fetchedContent
	auto mappingResult = Core::mapToProduct(*fetchedContent);
	if (!mappingResult) {
		qCritical() << mappingResult.error();
		return;
	}

	m_NewProduct = std::move(*mappingResult);

	qDebug() << "App received Product";
	qDebug() << "-----------------------";

	auto savingResult = Settings::saveProduct(*m_NewProduct);
	if (!savingResult) {
		qCritical() << savingResult.error();
		return;
	}
}

void App::showProductInfo()
{
	if (!m_NewProduct)
		return;

	const Core::Product& product = m_NewProduct.value();
	qDebug() << product.name();
	qDebug() << product.currentPrice();
	qDebug() << product.onSale();
	qDebug() << product.regularPrice();
	qDebug() << product.available();
	qDebug() << product.url();
	/*for (const auto& url : product.imageUrls()) {
		qDebug() << url;
	}*/
	qDebug() << "-----------------------";
}

// NOTE: Before every return we must commitNewProduct() - except when m_NewProduct is std::nullopt
void App::compareProducts()
{
	if (!m_NewProduct) {
		qCritical() << "New product is set to std::nullopt - something went wrong!";
		return;
	}

	// Alias for easier reading
	const Core::Product& newProduct = m_NewProduct.value();

	if (!m_LastProduct) {
		qWarning() << "Couldn't read last product info - it does not exist yet or something went wrong";

		// Name
		qDebug() << newProduct.name() << "info:";

		// Available
		if (newProduct.available())
			qDebug() << "Product is available";
		else
			qDebug() << "Product is not available";

		// Sale and price
		if (newProduct.onSale()) {
			qDebug() << "Product is on sale!";
			logPrice("Regular price:", price(newProduct.regularPrice()));
		}
		logPrice("Current price:", price(newProduct.currentPrice()));

		commitNewProduct();
		return;
	}

	// 2nd alias
	const Core::Product& lastProduct = m_LastProduct.value();

	// --------------------------------------------------------
	// 1. Check product names just in case
	// 2. Check if the product is available
	//    If not, reset product variables and return
	//    If yes, we compare other values
	//
	// The only info that needs comparing, checking all of the
	// possibilities and informing the user are:
	// availability, sale and price
	// --------------------------------------------------------

	// Name
	if (newProduct.name() != lastProduct.name())
		qWarning() << "Product names don't match - something might have gone wrong";
	qDebug() << newProduct.name() << "info:";

	// Available
	const bool wasAvailable = lastProduct.available();
	if (!newProduct.available()) {
		if (wasAvailable)
			qDebug() << "Product is no longer available";
		else
			qDebug() << "Product is still not available";

		commitNewProduct();
		return;
	}
	else {
		if (!wasAvailable) qDebug() << "Product is now available";
		else qDebug() << "Product is still available";
	}

	// Sale (+ regular price) and price
	// if product is on sale, we don't want to compare with previous price,
	// we compare with regular price specified in JSON instead
	const bool wasOnSale = lastProduct.onSale();
	if (newProduct.onSale()) {
		// New sale
		if (!wasOnSale)
			qDebug() << "Product is now on sale!";
		// Sale still lasts
		else qDebug() << "Product is still on sale";

		logPrice("Regular price:", price(newProduct.regularPrice()));
		logPrice("Current price:", price(newProduct.currentPrice()));
	}
	else {
		if (wasOnSale) qDebug() << "Product is no longer on sale";

		// Price
		const int oldPrice = lastProduct.currentPrice();
		const int newPrice = newProduct.currentPrice();
		if (newPrice == oldPrice)
			logPrice("Price didn't change:", price(newPrice));
		else {
			if (newPrice < oldPrice) {
				const int difference = (oldPrice - newPrice) / 100;
				qDebug() << "Price is lower by:" << difference << "PLN";
			}
			else {
				const int difference = (newPrice - oldPrice) / 100;
				qDebug() << "Price is higher by:" << difference << "PLN";
			}

			logPrice("Previous price:", price(lastProduct.currentPrice()));
			logPrice("Current price:", price(newProduct.currentPrice()));
		}
	}

	// Url
	const QUrl oldUrl = lastProduct.url();
	const QUrl newUrl = newProduct.url();
	if (newUrl != oldUrl) qWarning() << "Product's URL has changed";

	commitNewProduct();
}

void App::commitNewProduct()
{
	m_LastProduct = m_NewProduct.value();
	m_NewProduct = std::nullopt;
}
