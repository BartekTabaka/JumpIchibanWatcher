#pragma once
#include <QApplication>

#include "Core/Mapper.h"
#include "NetworkManager.h"
#include <optional>

// -- Forward -----------------
class App;
extern App *g_App;

// -- App class ---------------
class App : public QObject {
	Q_OBJECT
public:
	explicit App(QApplication& app);

	void createProduct();
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
