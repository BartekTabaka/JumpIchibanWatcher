#pragma once
#include <QApplication>

// -- Forward -----------------
class App;
extern App *g_App;

// -- App class ---------------
class App : public QObject {
	Q_OBJECT
public:
	explicit App(QApplication& app);
private:
	QApplication& m_App;
};
