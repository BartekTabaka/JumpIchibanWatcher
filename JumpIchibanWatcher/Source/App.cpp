#include "App.h"
#include <QDebug>
#include "Core/Product.h"

App *g_App = nullptr;

App::App(QApplication& app) : m_App(app)
{
	qDebug() << "The app is working!";
}

void App::createProduct()
{
	Core::Product prod("Kitagawa", 1274.00f, true, "https://example.com", { "https://example.com" }, 1275.99f);
}
