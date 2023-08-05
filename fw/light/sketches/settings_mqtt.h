#pragma once

#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdbool>

#include <string>

#include "types.h"
#include "settings_base.h"

class MqttSettings : public SettingsGroupBase
{
public:
	MqttSettings();
		
	SettingValueSource getEnabled(bool& result_out) const;
	void setEnabled(bool value);
	
	SettingValueSource getHost(std::string& result_out) const;
	void setHost(const std::string& value); 
	
	SettingValueSource getPort(int& result_out) const;
	void setPort(int value);
	
	SettingValueSource getUser(std::string& result_out) const;
	void setUser(const std::string& value);
	
	SettingValueSource getPassword(std::string& result_out) const;
	void setPassword(const std::string& value);
		
	SettingValueSource getInputControlTopic(std::string& result_out) const;
	void setInputControlTopic(const std::string& value);
	
	SettingValueSource getOutputControlTopic(std::string& result_out) const;
	void setOutputControlTopic(const std::string& value);
	
	SettingValueSource getIdentifier(std::string& result_out) const;
	void setIdentifier(const std::string& value);
	
private:
	
	static const std::string FIELD_ENABLED;
	static const std::string FIELD_USER;
	static const std::string FIELD_PASSWORD;
	static const std::string FIELD_HOST;
	static const std::string FIELD_PORT;
	static const std::string FIELD_INPUT_TOPIC;
	static const std::string FIELD_OUTPUT_TOPIC;
	static const std::string FIELD_IDENTIFIER;
};