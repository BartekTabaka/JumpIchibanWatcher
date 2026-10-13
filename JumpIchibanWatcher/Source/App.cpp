#include "App.h"
#include <QApplication>

#include "Core/Product.h"
#include "Core/Mapper.h"
#include "Settings.h"

#include <optional>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QList>
#include <QString>
#include <QtTypes>
#include <QUrl>
#include <utility>

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

	const QUrl kShopBaseUrl("https://jumpichiban.com");

	// Helper for now
	QUrl getUrl()
	{
		QUrl url = kShopBaseUrl;
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

	const QString kFeaturedImageFileName = QStringLiteral("image_featured.png");
} // namespace

App *g_App = nullptr;

App::App(QApplication& app) : m_App(app),
							  m_LastProduct(loadLastProductOrLog())
{
	if (!m_LastProduct)
		qWarning() << "Loading last product info from settings failed!";
	else
		qDebug() << "Loaded last product info from settings";

	// 10s for debugging purposes, 2mins minimum should be good in release version
	m_Timer.setInterval(10000); // 10s
	m_Timer.callOnTimeout(this, &App::executeWorkflow);

	qDebug() << "The app is working!";
	qDebug() << "-----------------------";
}

void App::run()
{
	executeWorkflow();
	m_Timer.start();
}

void App::downloadImages()
{
	if (!m_NewProduct) return;

	const Core::Product& product = m_NewProduct.value();

	saveImage(product.featuredImageUrl(), QDir(m_ImageDirName).filePath(kFeaturedImageFileName));

	int imageIndex = 0;
	for (const auto& url : product.imageUrls()) {
		const QString path = QDir(m_ImageDirName).filePath(QStringLiteral("image_%1.png").arg(++imageIndex));
		saveImage(url, path);
	}
}

bool App::areProductImagesSaved()
{
	if (!m_LastProduct) return false;
	const Core::Product& lastProduct = m_LastProduct.value();

	// Check if "images" directory exist
	const QDir workingDir;
	if (!workingDir.exists(m_ImageDirName)) {
		qDebug() << "Images directory not found!";
		return false;
	}

	const QDir imageDir(m_ImageDirName);

	// Check if the featured image exists
	const QString featuredImageFilePath = imageDir.filePath(kFeaturedImageFileName);
	if (!QFile::exists(featuredImageFilePath)) {
		qDebug() << "Featured image file not found!";
		return false;
	}

	// Check if other images exist
	const QList<QUrl>& imageList = lastProduct.imageUrls();
	for (qsizetype i = 0; i < imageList.size(); ++i) {
		const int imageIndex = i + 1;
		const QString imageFilePath = imageDir.filePath(QStringLiteral("image_%1.png").arg(imageIndex));

		if (!QFile::exists(imageFilePath)) {
			qDebug() << QStringLiteral("Image file not found! (%1)").arg(imageIndex);
			return false;
		}
	}

	return true;
}

void App::executeWorkflow()
{
	if (m_NetworkManager.isRequestPending()) {
		qDebug() << "Previous cycle didn't finish yet - skipping this cycle";
		return;
	}

	refreshProduct();
	compareProducts();

	if (!areProductImagesSaved())
		downloadImages();

	commitNewProduct();
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

	// Modify URL
	QUrl targetUrl = getUrl();
	targetUrl.setPath(targetUrl.path() + ".js");
	
	auto fetchedContent = m_NetworkManager.fetchData(targetUrl);
	if (!fetchedContent) {
		const auto& error = fetchedContent.error();
		
		// TODO (when a window will be introduced)
		qCritical() << error;
		if (error.code == NetworkErrorCode::NotFound) {
			// Product not found
		}
		else if (error.isRetryable()) {
			// Retry
		}
		
		return;
	}

	qDebug() << "App fetched raw content";
	qDebug() << "-----------------------";

	// NOTE: fetchedContent.value() == *fetchedContent
	auto mappingResult = Core::mapToProduct(*fetchedContent, kShopBaseUrl);
	if (!mappingResult) {
		qCritical() << mappingResult.error();
		return;
	}

	m_NewProduct = std::move(*mappingResult);

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
		qDebug() << "-----------------------";

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
		qDebug() << "-----------------------";

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
	if (newProduct.url() != lastProduct.url()) qWarning() << "Product's URL has changed";

	// Image URLs
	if (newProduct.featuredImageUrl() != lastProduct.featuredImageUrl()) {
		qDebug() << "Featured image URL is different";
		downloadImages();
	}

	const QList<QUrl>& oldList = lastProduct.imageUrls();
	const QList<QUrl>& newList = newProduct.imageUrls();
	for (qsizetype i = 0; i < oldList.size() && i < newList.size(); ++i) {
		if (newList[i] != oldList[i]) {
			qDebug() << QStringLiteral("Image URL is different (%1)").arg(i + 1);
			downloadImages();
			break;
		}
	}

	qDebug() << "-----------------------";
}

bool App::saveImage(const QUrl& url, const QString& path)
{
	if (path.isEmpty()) return false;

	auto fetchingResult = m_NetworkManager.fetchData(url);
	if (!fetchingResult) {
		qCritical() << "Fetching error:" << fetchingResult.error();
		return false;
	}

	QDir workingDir;
	if (!workingDir.exists(m_ImageDirName)) {
		if (!workingDir.mkpath(m_ImageDirName)) {
			qDebug() << "Couldn't create images directory!";
			return false;
		}
		else qDebug() << "Created images directory";
	}

	QFile file(path);
	if (!file.open(QIODevice::WriteOnly)) {
		qDebug() << QStringLiteral("Couldn't open image file (%1):").arg(path) << file.errorString();
		return false;
	}

	file.write(*fetchingResult);
	file.close();

	qDebug() << "Saved image in:" << path;
	return true;
}

void App::commitNewProduct()
{
	if (!m_NewProduct) return;

	m_LastProduct = m_NewProduct.value();
	m_NewProduct = std::nullopt;
}
