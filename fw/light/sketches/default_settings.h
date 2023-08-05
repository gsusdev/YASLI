#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdbool>
#include <cassert>

#include <string>
#include <map>
#include <algorithm>

#include <Arduino.h>
#include <FS.h>

#include "log_utils.h"

class DefaultSettingsCreator 
{
public:
	void setLogger(ILogger* value_p) { _logger.setLogger(value_p); }
	
	void setOverwriteIfExists(bool value) { _overwriteIfExists = value; }
	void setFileSystem(FS* value_p) { _fileSystem_p = value_p; }
	
	void setMainSettingsFilename(const String& value) { _mainSettingsFilename = value; }
	void setInputsFilename(const String& value) { _inputsFilename = value; }
	void setOutputsFilename(const String& value) { _outputsFilename = value; }
	void setRulesFilename(const String& value) { _rulesFilename = value; }
	
	bool execute();
	
private:
	bool writeText(const String& text, const String& filename, bool overwriteIfExists);
	
	LoggerHelper _logger;
	
	FS* _fileSystem_p = nullptr;
	
	String _mainSettingsFilename;
	String _inputsFilename;
	String _outputsFilename;
	String _rulesFilename;
	
	bool _overwriteIfExists = false;
	
	static const String _mainSettingsText;
	static const String _inputsText;
	static const String _outputsText;
	static const String _rulesText;
};