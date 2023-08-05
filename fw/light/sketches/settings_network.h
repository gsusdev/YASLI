#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdbool>

#include <string>

#include "types.h"
#include "settings_base.h"

class NetworkSettings : public SettingsGroupBase
{
public:
	enum class Interface
	{
		wifi, 
		ethernet
	};
			
	NetworkSettings();
		
	SettingValueSource getSsdpName(std::string& result_out) const;
	void setSsdpName(const std::string& value);
	
	SettingValueSource getHost(std::string& result_out) const;
	void setHost(const std::string& value);
	
	SettingValueSource getDhcpEnabled(bool& result_out) const;
	void setDhcpEnabled(bool value);
	
	SettingValueSource getStaticIp(std::string& result_out) const;
	void setStaticIp(const std::string& value);
	
	SettingValueSource getStaticMask(std::string& result_out) const;
	void setStaticMask(const std::string& value);
	
	SettingValueSource getStaticGateway(std::string& result_out) const;
	void setStaticGateway(const std::string& value);
	
	SettingValueSource getInterface(Interface& result_out) const;
	void setInterface(Interface value);
		
private:
	class InterfaceValue : public EnumSettingValue<Interface>
	{
	private:
		const std::map<Interface, std::vector<std::string>>& designators() const override;	
	};
	
	static const std::string FIELD_INTERFACE;
	static const std::string FIELD_SSDP_NAME;
	static const std::string FIELD_HOST;
	static const std::string FIELD_DHCP_ENABLED;
	static const std::string FIELD_STATIC_IP;
	static const std::string FIELD_STATIC_MASK;
	static const std::string FIELD_STATIC_GATEWAY;
};