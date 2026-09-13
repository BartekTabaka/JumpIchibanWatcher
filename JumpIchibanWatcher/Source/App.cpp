#include "App.h"

#include "Settings.h"
#include <QEventLoop>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

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
	QUrl BuildUrl()
	{
		QUrl url;
		url.setScheme("https");
		url.setHost("jumpichiban.com");
		url.setPath("/products/ichiban-kuji-my-dress-up-darling-season-2-last-one-prize-marin-kitagawa-artscale-memoria-figure.js");

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

	m_NetworkManager.setTransferTimeout(15000); // 15s
}

void App::createProduct()
{
	// Hard-coded JSON just for now
	QByteArray fetchedContent = R"(
	{
		"id": 9130968973566,
		"title": "Ichiban Kuji My Dress-Up Darling Season 2 - Last One Prize Marin Kitagawa ArtScale Memoria Figure",
		"handle": "ichiban-kuji-my-dress-up-darling-season-2-last-one-prize-marin-kitagawa-artscale-memoria-figure",
		"description": "\u003cdiv\u003e\n\u003ch2\u003e\u003cstrong\u003eICHIBAN KUJI MY DRESS-UP DARLING SEASON 2 - LAST ONE PRIZE MARIN KITAGAWA ARTSCALE MEMORIA FIGURE\u003c\/strong\u003e\u003c\/h2\u003e\n\u003cspan\u003e\u003c\/span\u003e\n\u003c\/div\u003e\n\u003cdiv\u003e\u003cspan\u003e\u003c\/span\u003e\u003c\/div\u003e\n\u003cdiv\u003e\u003cbr\u003e\u003c\/div\u003e\n\u003cdiv\u003ePlease be sure to add this My Dress-Up Darling figure to your collection!\u003cbr\u003e\n\u003cp\u003e\u003cspan data-contrast=\"none\"\u003e\u003c!-- show up to 2 reviews by default --\u003e\u003c\/span\u003e\u003c\/p\u003e\n\u003cdiv class=\"gs__list__box cf\"\u003e\n\u003ctable width=\"100%\" style=\"width: 100%; height: 556.96px;\"\u003e\n\u003ctbody\u003e\n\u003ctr style=\"height: 33.4801px;\"\u003e\n\u003ctd style=\"width: 33.7638%; height: 33.4801px;\"\u003e\n\u003ch3\u003e\u003cstrong\u003ePRODUCT DETAILS\u003c\/strong\u003e\u003c\/h3\u003e\n\u003c\/td\u003e\n\u003ctd style=\"width: 65.8672%; height: 33.4801px;\"\u003e\u003cspan style=\"color: #808080;\"\u003e\u003cspan style=\"color: #000000;\"\u003e\u003cbr\u003e\u003c\/span\u003e\u003c\/span\u003e\u003c\/td\u003e\n\u003c\/tr\u003e\n\u003ctr style=\"height: 39.2045px;\"\u003e\n\u003ctd style=\"width: 33.7638%; height: 39.2045px;\"\u003e\u003cspan style=\"color: #808080;\"\u003eProduct Name \u003c\/span\u003e\u003c\/td\u003e\n\u003ctd style=\"width: 65.8672%; height: 39.2045px;\"\u003eIchiban Kuji My Dress-Up Darling Season 2 - Last One Prize Marin Kitagawa ArtScale Memoria Figure\u003c\/td\u003e\n\u003c\/tr\u003e\n\u003ctr style=\"height: 156.747px;\"\u003e\n\u003ctd style=\"width: 33.7638%; height: 156.747px;\"\u003e\u003cspan style=\"color: rgb(128, 128, 128);\"\u003eMedia\u003c\/span\u003e\u003c\/td\u003e\n\u003ctd style=\"width: 65.8672%; height: 156.747px;\"\u003eFigure\u003c\/td\u003e\n\u003c\/tr\u003e\n\u003ctr style=\"height: 39.1761px;\"\u003e\n\u003ctd style=\"width: 33.7638%; height: 39.1761px;\"\u003e\u003cspan style=\"color: rgb(128, 128, 128);\"\u003ePackage Content\u003c\/span\u003e\u003c\/td\u003e\n\u003ctd style=\"width: 65.8672%; height: 39.1761px;\"\u003e1x Figure (official Japanese figure brand new)\u003c\/td\u003e\n\u003c\/tr\u003e\n\u003ctr style=\"height: 19.6023px;\"\u003e\n\u003ctd style=\"width: 33.7638%; height: 19.6023px;\"\u003e\u003cspan style=\"color: rgb(128, 128, 128);\"\u003eRelease Date\u003c\/span\u003e\u003c\/td\u003e\n\u003ctd style=\"width: 65.8672%; height: 19.6023px;\"\u003eFebruary 12, 2026\u003c\/td\u003e\n\u003c\/tr\u003e\n\u003ctr style=\"height: 19.6023px;\"\u003e\n\u003ctd style=\"width: 33.7638%; height: 19.6023px;\"\u003e\u003cspan style=\"color: rgb(128, 128, 128);\"\u003eManufacturer\u003c\/span\u003e\u003c\/td\u003e\n\u003ctd style=\"width: 65.8672%; height: 19.6023px;\"\u003e\u003cspan data-contrast=\"none\"\u003e\u003cspan style=\"color: #808080;\"\u003e\u003cspan style=\"color: #000000;\"\u003eBandai\u003c\/span\u003e\u003c\/span\u003e\u003c\/span\u003e\u003c\/td\u003e\n\u003c\/tr\u003e\n\u003ctr style=\"height: 35.1847px;\"\u003e\n\u003ctd style=\"width: 33.7638%; height: 35.1847px;\"\u003e\u003cspan style=\"color: rgb(128, 128, 128);\"\u003eSize\u003c\/span\u003e\u003c\/td\u003e\n\u003ctd style=\"width: 65.8672%; height: 35.1847px;\"\u003e\u003cspan style=\"color: #000000;\"\u003e\u003cspan\u003eApprox. 18 cm\u003c\/span\u003e\u003c\/span\u003e\u003c\/td\u003e\n\u003c\/tr\u003e\n\u003ctr style=\"height: 19.6023px;\"\u003e\n\u003ctd style=\"width: 33.7638%; height: 19.6023px;\"\u003e\u003cspan style=\"color: rgb(128, 128, 128);\"\u003eProduct Line\u003c\/span\u003e\u003c\/td\u003e\n\u003ctd style=\"width: 65.8672%; height: 19.6023px;\"\u003e\u003cspan data-contrast=\"none\"\u003e\u003cspan style=\"color: #808080;\"\u003e\u003cspan style=\"color: #000000;\"\u003eIchiban Kuji, \u003cspan data-sheets-root=\"1\"\u003eArtScale Memoria\u003c\/span\u003e\u003c\/span\u003e\u003c\/span\u003e\u003c\/span\u003e\u003c\/td\u003e\n\u003c\/tr\u003e\n\u003ctr style=\"height: 97.9688px;\"\u003e\n\u003ctd style=\"width: 33.7638%; height: 97.9688px;\"\u003e\u003cspan style=\"color: rgb(128, 128, 128);\"\u003eLanguage\u003c\/span\u003e\u003c\/td\u003e\n\u003ctd style=\"width: 65.8672%; height: 97.9688px;\"\u003e\n\u003cdiv class=\"gs__list__box__2\"\u003eJapanese\u003c\/div\u003e\n\u003c\/td\u003e\n\u003c\/tr\u003e\n\u003ctr style=\"height: 19.6023px;\"\u003e\n\u003ctd style=\"width: 33.7638%; height: 19.6023px;\"\u003e\u003cspan style=\"color: rgb(128, 128, 128);\"\u003eMaterials\u003c\/span\u003e\u003c\/td\u003e\n\u003ctd style=\"width: 65.8672%; height: 19.6023px;\"\u003e\n\u003cdiv class=\"gs__list__box__2\"\u003ePVC, ABS\u003c\/div\u003e\n\u003c\/td\u003e\n\u003c\/tr\u003e\n\u003ctr style=\"height: 37.5852px;\"\u003e\n\u003ctd style=\"width: 33.7638%; height: 37.5852px;\"\u003e\u003cspan style=\"color: rgb(128, 128, 128);\"\u003eThemes\u003c\/span\u003e\u003c\/td\u003e\n\u003ctd style=\"width: 65.8672%; height: 37.5852px;\"\u003e\u003cspan data-contrast=\"none\"\u003eManga \u0026amp; Animations\u003c\/span\u003e\u003c\/td\u003e\n\u003c\/tr\u003e\n\u003ctr style=\"height: 19.6023px;\"\u003e\n\u003ctd style=\"width: 33.7638%; height: 19.6023px;\"\u003e\u003cspan style=\"color: rgb(128, 128, 128);\"\u003eCharacter\u003c\/span\u003e\u003c\/td\u003e\n\u003ctd style=\"width: 65.8672%; height: 19.6023px;\"\u003e\u003cspan data-sheets-root=\"1\"\u003eMarin Kitagawa\u003c\/span\u003e\u003c\/td\u003e\n\u003c\/tr\u003e\n\u003ctr style=\"height: 19.6023px;\"\u003e\n\u003ctd style=\"width: 33.7638%; height: 19.6023px;\"\u003e\u003cspan style=\"color: rgb(128, 128, 128);\"\u003eSeries\u003c\/span\u003e\u003c\/td\u003e\n\u003ctd style=\"width: 65.8672%; height: 19.6023px;\"\u003e\n\u003cdiv class=\"gs__list__box__2\"\u003e\u003cspan data-contrast=\"none\"\u003e\u003cspan style=\"color: #808080;\"\u003e\u003cspan style=\"color: #000000;\"\u003e\u003cspan style=\"color: #404040;\"\u003e\u003cspan style=\"color: #000000;\"\u003e\u003cspan data-sheets-root=\"1\"\u003eMy Dress-Up Darling\u003c\/span\u003e\u003c\/span\u003e\u003c\/span\u003e\u003c\/span\u003e\u003c\/span\u003e\u003c\/span\u003e\u003c\/div\u003e\n\u003c\/td\u003e\n\u003c\/tr\u003e\n\u003c\/tbody\u003e\n\u003c\/table\u003e\n\u003c\/div\u003e\n\u003cul style=\"font-weight: 400;\"\u003e\u003c\/ul\u003e\n\u003c\/div\u003e\n\u003cp\u003e\u003ca href=\"https:\/\/jumpichiban.com\/collections\/my-dress-up-darling-series\"\u003e\u003cimg alt=\"\" src=\"https:\/\/cdn.shopify.com\/s\/files\/1\/0657\/6164\/0702\/files\/My_Dress_Up_Darling_Japanese_Logo.webp?v=1740201342\" width=\"271\" height=\"105\" style=\"display: block; margin-left: auto; margin-right: auto;\"\u003e\u003c\/a\u003e\u003c\/p\u003e",
		"published_at": "2026-03-08T21:17:47+09:00",
		"created_at": "2026-02-11T10:12:33+09:00",
		"vendor": "JumpIchiban",
		"type": "",
		"tags": [],
		"price": 127600,
		"price_min": 127600,
		"price_max": 127600,
		"available": true,
		"price_varies": false,
		"compare_at_price": null,
		"compare_at_price_min": 0,
		"compare_at_price_max": 0,
		"compare_at_price_varies": false,
		"variants": [
			{
				"id": 47789768081662,
				"title": "Default Title",
				"option1": "Default Title",
				"option2": null,
				"option3": null,
				"sku": null,
				"requires_shipping": true,
				"taxable": true,
				"featured_image": null,
				"available": true,
				"name": "Ichiban Kuji My Dress-Up Darling Season 2 - Last One Prize Marin Kitagawa ArtScale Memoria Figure",
				"public_title": null,
				"options": [
					"Default Title"
				],
				"price": 127600,
				"weight": 9100,
				"compare_at_price": null,
				"inventory_management": "shopify",
				"barcode": null,
				"quantity_rule": {
					"min": 1,
					"max": null,
					"increment": 1
				},
				"quantity_price_breaks": [],
				"requires_selling_plan": false,
				"selling_plan_allocations": []
			}
		],
		"images": [
			"\/\/cdn.shopify.com\/s\/files\/1\/0657\/6164\/0702\/files\/IchibanKujiMyDress-UpDarlingSeason2-LastOnePrizeMarinKitagawaArtScaleMemoriaFigure_2.jpg?v=1770772578",
			"\/\/cdn.shopify.com\/s\/files\/1\/0657\/6164\/0702\/files\/IchibanKujiMyDress-UpDarlingSeason2-LastOnePrizeMarinKitagawaArtScaleMemoriaFigure_3.jpg?v=1770772578",
			"\/\/cdn.shopify.com\/s\/files\/1\/0657\/6164\/0702\/files\/IchibanKujiMyDress-UpDarlingSeason2-LastOnePrizeMarinKitagawaArtScaleMemoriaFigure_1.jpg?v=1770772578"
		],
		"featured_image": "\/\/cdn.shopify.com\/s\/files\/1\/0657\/6164\/0702\/files\/IchibanKujiMyDress-UpDarlingSeason2-LastOnePrizeMarinKitagawaArtScaleMemoriaFigure_2.jpg?v=1770772578",
		"options": [
			{
				"name": "Title",
				"position": 1,
				"values": [
					"Default Title"
				]
			}
		],
		"url": "\/products\/ichiban-kuji-my-dress-up-darling-season-2-last-one-prize-marin-kitagawa-artscale-memoria-figure",
		"media": [
			{
				"alt": null,
				"id": 38377498149118,
				"position": 1,
				"preview_image": {
					"aspect_ratio": 1.0,
					"height": 1000,
					"width": 1000,
					"src": "https:\/\/cdn.shopify.com\/s\/files\/1\/0657\/6164\/0702\/files\/IchibanKujiMyDress-UpDarlingSeason2-LastOnePrizeMarinKitagawaArtScaleMemoriaFigure_2.jpg?v=1770772578"
				},
				"aspect_ratio": 1.0,
				"height": 1000,
				"media_type": "image",
				"src": "https:\/\/cdn.shopify.com\/s\/files\/1\/0657\/6164\/0702\/files\/IchibanKujiMyDress-UpDarlingSeason2-LastOnePrizeMarinKitagawaArtScaleMemoriaFigure_2.jpg?v=1770772578",
				"width": 1000
			},
			{
				"alt": null,
				"id": 38377498181886,
				"position": 2,
				"preview_image": {
					"aspect_ratio": 1.0,
					"height": 1000,
					"width": 1000,
					"src": "https:\/\/cdn.shopify.com\/s\/files\/1\/0657\/6164\/0702\/files\/IchibanKujiMyDress-UpDarlingSeason2-LastOnePrizeMarinKitagawaArtScaleMemoriaFigure_3.jpg?v=1770772578"
				},
				"aspect_ratio": 1.0,
				"height": 1000,
				"media_type": "image",
				"src": "https:\/\/cdn.shopify.com\/s\/files\/1\/0657\/6164\/0702\/files\/IchibanKujiMyDress-UpDarlingSeason2-LastOnePrizeMarinKitagawaArtScaleMemoriaFigure_3.jpg?v=1770772578",
				"width": 1000
			},
			{
				"alt": null,
				"id": 38377498214654,
				"position": 3,
				"preview_image": {
					"aspect_ratio": 1.0,
					"height": 1000,
					"width": 1000,
					"src": "https:\/\/cdn.shopify.com\/s\/files\/1\/0657\/6164\/0702\/files\/IchibanKujiMyDress-UpDarlingSeason2-LastOnePrizeMarinKitagawaArtScaleMemoriaFigure_1.jpg?v=1770772578"
				},
				"aspect_ratio": 1.0,
				"height": 1000,
				"media_type": "image",
				"src": "https:\/\/cdn.shopify.com\/s\/files\/1\/0657\/6164\/0702\/files\/IchibanKujiMyDress-UpDarlingSeason2-LastOnePrizeMarinKitagawaArtScaleMemoriaFigure_1.jpg?v=1770772578",
				"width": 1000
			}
		],
		"requires_selling_plan": false,
		"selling_plan_groups": []
	})";

	///////////////////////////////////////////////////////////////////////////////////////////
	// NOTE: .transform should be used with function chain, for now it's only doing 1 thing, //
	// so I decided to go with simple *if* for now											 //
	///////////////////////////////////////////////////////////////////////////////////////////

	auto fetched = fetchProductJson();
	if (fetched) {
		qDebug() << "Fetched content:";
		qDebug().noquote() << *fetched;
		qDebug() << "-----------------------";
	}
	else {
		qCritical() << "Fetching error:" << fetched.error();
	}

	auto mappingResult = Core::mapToProduct(fetchedContent);
	if (mappingResult) {
		m_NewProduct = mappingResult.value();
		qDebug() << "App received Product";
		qDebug() << "-----------------------";

		Settings::SaveNewProduct(m_NewProduct.value());
	}
	else {
		qCritical() << mappingResult.error();
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
		qDebug() << "Product names don't match - something might have gone wrong";

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
			if (newPrice < oldPrice) qDebug() << "Price is lower";
			else qDebug() << "Price is higher";

			LogPrice("Previous price:", Price(lastProduct.currentPrice()));
			LogPrice("Current price:", Price(newProduct.currentPrice()));
		}
	}

	// Url
	const QString oldUrl = lastProduct.url();
	const QString newUrl = newProduct.url();
	if (newUrl != oldUrl) qDebug() << "Product's URL has changed";

	commitNewProduct();
}

std::expected<QByteArray, QString> App::fetchProductJson()
{
	// Request and headers
	QNetworkRequest request(BuildUrl());
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

void App::commitNewProduct()
{
	m_LastProduct = m_NewProduct.value();
	m_NewProduct = std::nullopt;
}
