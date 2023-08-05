#include "settings_ftp.h"

const std::string FtpSettings::FIELD_ENABLED = "enabled";
const std::string FtpSettings::FIELD_USER = "user";
const std::string FtpSettings::FIELD_PASSWORD = "password";
const std::string FtpSettings::FIELD_PORT = "port";

FtpSettings::FtpSettings()
	: SettingsGroupBase()
{
	auto& enabled = addItem<BooleanSettingValue>(FIELD_ENABLED);
	enabled.setCaption("Enabled");
	enabled.setIsRequired(true);
	
	auto& user = addItem<StringSettingValue>(FIELD_USER);
	user.setCaption("User");
	user.setIsRequired(false);
		
	auto& password = addItem<StringSettingValue>(FIELD_PASSWORD);
	password.setCaption("Password");
	password.setIsRequired(false);
	
	auto& port = addItem<IntegerSettingValue>(FIELD_PORT);
	port.setCaption("Port");
	port.setDefaultValue(0); 
	port.setIsRequired(false);
}

SettingValueSource FtpSettings::getUser(std::string& result_out) const
{
	const auto result = item<StringSettingValue>(FIELD_USER).get(result_out);
	return result;
}

void FtpSettings::setUser(const std::string& value)
{
	item<StringSettingValue>(FIELD_USER).set(value);
}

SettingValueSource FtpSettings::getPassword(std::string& result_out) const
{
	const auto result = item<StringSettingValue>(FIELD_PASSWORD).get(result_out);
	return result;
}

void FtpSettings::setPassword(const std::string& value)
{
	item<StringSettingValue>(FIELD_PASSWORD).set(value);
}
	
SettingValueSource FtpSettings::getPort(int& result_out) const
{
	const auto result = item<IntegerSettingValue>(FIELD_PORT).get(result_out);
	return result;
}

void FtpSettings::setPort(int value)
{
	item<IntegerSettingValue>(FIELD_PORT).set(value);
}

SettingValueSource FtpSettings::getEnabled(bool& result_out) const
{
	const auto result = item<BooleanSettingValue>(FIELD_ENABLED).get(result_out);
	return result;
}

void FtpSettings::setEnabled(bool value)
{
	item<BooleanSettingValue>(FIELD_ENABLED).set(value);
}