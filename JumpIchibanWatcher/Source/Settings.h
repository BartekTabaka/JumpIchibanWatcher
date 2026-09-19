#pragma once
#include <QSettings>

#include <optional>
#include "Core/Product.h"

// Static-only wrapper around QSettings for the app's persisted preferences.
// Never instantiated; see Settings.cpp for storage location and defaults.
class Settings {
public:
	// Application settings (future commits)

	// Product
	static std::optional<Core::Product> loadLastProduct();
	static void saveNewProduct(const Core::Product& product);
};
