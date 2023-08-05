#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdbool>

#include <string>

#include <Arduino.h>
#include <FS.h>

#include "IniFile.h"

#include "types.h"
#include "log_utils.h"

class SettingsReaderIni 
{
public:
	~SettingsReaderIni();
	
	void setLogger(ILogger* value_p) { _logger.setLogger(value_p); }	
	void setFileSystem(FS* value_p) { _fileSystem_p = value_p; }
	
	bool open(const String& filename);
	void close();
	
	bool readGroup(const std::string& iniGroupName, ISettingsGroup& destination);
	
private:
	LoggerHelper _logger;
	
	FS* _fileSystem_p = nullptr;
	IniFile* _ini_p = nullptr;
	
	char _buf[128];
};