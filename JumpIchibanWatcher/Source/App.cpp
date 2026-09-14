#include "App.h"

#include "Settings.h"

namespace
{
	// TOOD: add currencies

	// Price wrapper
	QString Price(int cents)
	{
		double full = (static_cast<double>(cents) / 100);
		return QString::number(full, 'f', 2) + " PLN";
	}

	// Logging helper
	void LogPrice(const QString& label, const QString& value)
	{
		QDebug dbg = qDebug();
		QDebugStateSaver saver(dbg);
		dbg.noquote();

		dbg << label << value;
	}

	// Helper for now
	QUrl GetUrl()
	{
		QUrl url;
		url.setScheme("https");
		url.setHost("jumpichiban.com");
		url.setPath("/products/ichiban-kuji-my-dress-up-darling-season-2-last-one-prize-marin-kitagawa-artscale-memoria-figure");
		// The user will simply copy the url of HTML site, '.js' will be added inside the NetworkManager class

		return url;
	}
} // namespace

App *g_App = nullptr;

App::App(QApplication& app) : m_App(app),
							  m_LastProduct(Settings::LoadLastProduct())
{
	qDebug() << "Loaded last product from settings";
	qDebug() << "The app is working!";
	qDebug() << "-----------------------";
}

void App::createProduct()
{
	////////////////////////////////////////////////////////////////////////////////////
	// NOTE: .transform should be used for function chains.						  	  //
	// For now, it's only doing 1 thing, so I decided to go with simple if statement. //
	////////////////////////////////////////////////////////////////////////////////////
	
	auto fetchedContent = m_NetworkManager.fetchProductJson(GetUrl());
	if (!fetchedContent) {
		qCritical() << "Fetching error:" << fetchedContent.error();
		return;
	}

	// fetchedContent.value() == *fetchedContent
	auto mappingResult = Core::mapToProduct(*fetchedContent);
	if (!mappingResult) {
		qCritical() << mappingResult.error();
		return;
	}

	m_NewProduct = *mappingResult;

	qDebug() << "App received Product";
	qDebug() << "-----------------------";

	Settings::SaveNewProduct(*m_NewProduct);
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

void App::compareProducts()
{
	if (!m_NewProduct || !m_LastProduct) 
		return;

	// Aliases for easier reading
	const Core::Product& newProduct = m_NewProduct.value();
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

		LogPrice("Regular price:", Price(newProduct.regularPrice()));
		LogPrice("Current price:", Price(newProduct.currentPrice()));
	}
	else {
		if (wasOnSale) qDebug() << "Product is no longer on sale";

		// Price
		const int oldPrice = lastProduct.currentPrice();
		const int newPrice = newProduct.currentPrice();
		if (newPrice == oldPrice)
			LogPrice("Price didn't change:", Price(newPrice));
		else {
			if (newPrice < oldPrice) {
				const int difference = oldPrice - newPrice;
				qDebug() << "Price is lower by:" << difference;
			}
			else {
				const int difference = newPrice - oldPrice;
				qDebug() << "Price is higher by:" << difference;
			}

			LogPrice("Previous price:", Price(lastProduct.currentPrice()));
			LogPrice("Current price:", Price(newProduct.currentPrice()));
		}
	}

	// Url
	const QString oldUrl = lastProduct.url();
	const QString newUrl = newProduct.url();
	if (newUrl != oldUrl) qWarning() << "Product's URL has changed";

	commitNewProduct();
}

void App::commitNewProduct()
{
	m_LastProduct = m_NewProduct.value();
	m_NewProduct = std::nullopt;
}
