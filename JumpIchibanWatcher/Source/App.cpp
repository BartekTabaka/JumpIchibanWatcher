#include "App.h"
#include <QDebug>

App *g_App = nullptr;

App::App(QApplication& app) : m_App(app)
{
	qDebug() << "The app is working!";
}
