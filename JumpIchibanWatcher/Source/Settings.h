#pragma once
#include <QSettings>

#include <expected>
#include <QDebug>
#include <QString>
#include "Core/Product.h"

enum class SettingsErrorCode {
	FileNotFound,
	CannotOpenFile,
	InvalidFormat,
	//InvalidValueType,	TODO (#1) 
	MissingValue, // only returned by loadLastProduct()
	//MissingSetting <- don't need it yet, will introduce it with application settings
};

struct SettingsError {
	SettingsErrorCode code;
	QString message;

	friend QDebug operator<<(QDebug debug, SettingsError error)
	{
		debug << "SettingsError:";
		switch (error.code) {
		case SettingsErrorCode::FileNotFound:	  debug << "file not found"; break;
		case SettingsErrorCode::CannotOpenFile:   debug << "cannot open file"; break;
		case SettingsErrorCode::InvalidFormat:	  debug << "invalid format"; break;
		//case SettingsErrorCode::InvalidValueType: debug << "invalid value"; break;
		case SettingsErrorCode::MissingValue:	  debug << "missing value"; break;
		//case SettingsErrorCode::MissingSetting:   debug << "missing setting"; break;
		default:							  debug << "something went wrong - unknown error"; break;
		}
		if (!error.message.isEmpty()) {
			QDebugStateSaver saver(debug);
			debug.noquote() << '(' + error.message + ')';
		}

		return debug;
	}
};

// Static-only wrapper around QSettings for the app's persisted preferences.
// Never instantiated; see Settings.cpp for storage location and defaults.
class Settings {
public:
	// Application settings (future commits)

	// Product
	static std::expected<Core::Product, SettingsError> loadLastProduct();
	static std::expected<void, SettingsError> saveProduct(const Core::Product& product);
};
