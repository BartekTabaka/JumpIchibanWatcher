#pragma once
#include <QApplication>

#include <optional>
#include <QObject>
#include "Core/Mapper.h"
#include "Core/Product.h"
#include "NetworkManager.h"

// -- Forward -----------------
class App;
extern App *g_App;

// -- App class ---------------
class App : public QObject {
	Q_OBJECT
public:
	explicit App(QApplication& app);

	void refreshProduct();
	void showProductInfo();

	// void for now, comparing results will be shown
	// in console, might become a different type when
	// connecting the program with QML / QtWidgets
	void compareProducts();
private:
	void commitNewProduct();
private:
	QApplication& m_App;

	std::optional<Core::Product> m_LastProduct;
	std::optional<Core::Product> m_NewProduct;

	NetworkManager m_NetworkManager;
};
