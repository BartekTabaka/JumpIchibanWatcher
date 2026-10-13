#pragma once

#include "Core/Product.h"
#include "MainWindow.h"
#include "NetworkManager.h"

#include <optional>
#include <QObject>
#include <QString>
#include <QTimer>

// -- Forward -----------------
class App;
extern App *g_App;

class QApplication;
class QUrl;

// -- App class ---------------
class App : public QObject {
	Q_OBJECT
public:
	explicit App(QApplication& app);

	// Starts the main application workflow.
	void run();
private:
	// Executes the main application workflow
	void executeWorkflow();

	void refreshProduct();
	void showProductInfo();
	void downloadImages();
	bool areProductImagesSaved();

	// void for now, comparing results will be shown
	// in console, might become a different type when
	// connecting the program with QML / QtWidgets
	void compareProducts();

	// Temporarily saves an image to disk.
	// This will be replaced with on-demand image fetching once the UI is implemented.
	bool saveImage(const QUrl& url, const QString& path);
	void commitNewProduct();
private:
	QApplication& m_App;

	std::optional<Core::Product> m_LastProduct;
	std::optional<Core::Product> m_NewProduct;

	NetworkManager m_NetworkManager;
	QTimer m_Timer;

	const QString m_ImageDirName = "images";
};
