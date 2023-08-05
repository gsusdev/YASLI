#pragma once

#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdbool>

#include <string>

#include "types.h"
#include "settings_base.h"

class FtpSettings : public SettingsGroupBase
{
public:
	FtpSettings();
		
	SettingValueSource getEnabled(bool& result_out) const;
	void setEnabled(bool value);
	 
	SettingValueSource getPort(int& result_out) const;
	void setPort(int value);
	
	SettingValueSource getUser(std::string& result_out) const;
	void setUser(const std::string& value);
	
	SettingValueSource getPassword(std::string& result_out) const;
	void setPassword(const std::string& value);
		
private:
	
	static const std::string FIELD_ENABLED;
	static const std::string FIELD_USER;
	static const std::string FIELD_PASSWORD;
	static const std::string FIELD_PORT;
};