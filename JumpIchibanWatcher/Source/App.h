#pragma once
#include <QApplication>

#include "Core/Mapper.h"
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
private:
	QApplication& m_App;

	Core::Product m_LastProduct;
	std::optional<Core::Product> m_NewProduct;
};
