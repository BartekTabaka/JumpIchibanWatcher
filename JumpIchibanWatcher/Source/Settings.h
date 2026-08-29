#pragma once
#include <QSettings>

#include "Core/Product.h"

// Static-only wrapper around QSettings for the app's persisted preferences.
// Never instantiated; see Settings.cpp for storage location and defaults.
class Settings {
public:
	// Application settings (future commits)

	// Product
	static Core::Product LoadLastProduct();
	static void SaveNewProduct(Core::Product product);
};
