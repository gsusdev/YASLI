#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdbool>

#include <string>

#include "types.h"
#include "settings_base.h"

class WifiSettings : public SettingsGroupBase
{
public:
	enum class WifiRole
	{
		accessPoint,
		station
	}; 
			
	WifiSettings();
		
	SettingValueSource getSid(std::string& result_out) const;
	void setSid(const std::string& value);
	
	SettingValueSource getPassword(std::string& result_out) const;
	void setPassword(const std::string& value);
	
	SettingValueSource getRole(WifiRole& result_out) const;
	void setRole(WifiRole value);
		
private:
	class WifiRoleValue : public EnumSettingValue<WifiRole>
	{
	private:
		const std::map<WifiRole, std::vector<std::string>>& designators() const override;	
	};
	
	static const std::string FIELD_SID;
	static const std::string FIELD_PASSWORD;
	static const std::string FIELD_ROLE;
};