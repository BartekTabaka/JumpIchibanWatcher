#include "App.h"

#include <QDebug>

App *g_App = nullptr;

App::App(QApplication& app) : m_App(app)
{
	qDebug() << "The app is working!";
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
        "compare_at_price": 129099,
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

	Core::Product m_Product = Core::mapToProduct(fetchedContent);
}
