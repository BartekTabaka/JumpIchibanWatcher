#pragma once
#include <QApplication>
#include "Core/Mapper.h"

// -- Forward -----------------
class App;
extern App *g_App;

// -- App class ---------------
class App : public QObject {
	Q_OBJECT
public:
	explicit App(QApplication& app);

	void createProduct();
private:
	QApplication& m_App;
};
