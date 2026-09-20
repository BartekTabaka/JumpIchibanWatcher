#pragma once
#include <QSettings>

#include <expected>
#include <QDebug>
#include <QString>
#include "Core/Product.h"

enum class LoadErrorCode {
	FileNotFound,
	CannotOpenFile,
	InvalidFormat,
	//InvalidValueType,	TODO (#1) 
	MissingValue
	//MissingSetting <- don't need it yet, will introduce it with application settings
};

struct LoadError {
	LoadErrorCode code;
	QString message;

	friend QDebug operator<<(QDebug debug, LoadError error)
	{
		debug << "LoadError:";
		switch (error.code) {
		case LoadErrorCode::FileNotFound:	  debug << "file not found"; break;
		case LoadErrorCode::CannotOpenFile:   debug << "cannot open file"; break;
		case LoadErrorCode::InvalidFormat:	  debug << "invalid format"; break;
		//case LoadErrorCode::InvalidValueType: debug << "invalid value"; break;
		case LoadErrorCode::MissingValue:	  debug << "missing value"; break;
		//case LoadErrorCode::MissingSetting:   debug << "missing setting"; break;
		default:							  debug << "something went wrong - unknwon error"; break;
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
	static std::expected<Core::Product, LoadError> loadLastProduct();
	static void saveProduct(const Core::Product& product);
};
