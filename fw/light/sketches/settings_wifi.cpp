#include "settings_wifi.h"

const std::string WifiSettings::FIELD_SID = "ssid";
const std::string WifiSettings::FIELD_PASSWORD = "password";
const std::string WifiSettings::FIELD_ROLE = "mode";

const std::map<WifiSettings::WifiRole, std::vector<std::string>>& WifiSettings::WifiRoleValue::designators() const 
{ 
	static const std::map<WifiRole, std::vector<std::string>> result = 
	{
		{ WifiRole::accessPoint, { "access-point", "access_point", "accessPoint" } },
		{ WifiRole::station,     { "station", } }
	};
			
	return result; 
}		

WifiSettings::WifiSettings() :
	SettingsGroupBase()
{
	auto& role = addItem<WifiRoleValue>(FIELD_ROLE);
	role.setCaption("Wifi role");
	role.setIsRequired(true); 
		
	auto& sid = addItem<StringSettingValue>(FIELD_SID);
	sid.setCaption("SID");
	sid.setIsRequired(true);
		
	auto& password = addItem<StringSettingValue>(FIELD_PASSWORD);
	password.setCaption("Password");
	password.setIsRequired(true);
}

SettingValueSource WifiSettings::getSid(std::string& result_out) const
{
	const auto result = item<StringSettingValue>(FIELD_SID).get(result_out);
	return result;
}

void WifiSettings::setSid(const std::string& value)
{
	item<StringSettingValue>(FIELD_SID).set(value);
}
	
SettingValueSource WifiSettings::getPassword(std::string& result_out) const
{
	const auto result = item<StringSettingValue>(FIELD_PASSWORD).get(result_out);
	return result;
}

void WifiSettings::setPassword(const std::string& value)
{
	item<StringSettingValue>(FIELD_PASSWORD).set(value);
}
	
SettingValueSource WifiSettings::getRole(WifiRole& result_out) const
{
	const auto result = item<WifiRoleValue>(FIELD_ROLE).get(result_out);
	return result;
}

void WifiSettings::setRole(WifiRole value)
{
	item<WifiRoleValue>(FIELD_ROLE).set(value);
}