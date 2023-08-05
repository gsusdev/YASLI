#include "settings_network.h"

const std::string NetworkSettings::FIELD_INTERFACE      = "interface";
const std::string NetworkSettings::FIELD_SSDP_NAME      = "ssdp-name";
const std::string NetworkSettings::FIELD_HOST           = "host";
const std::string NetworkSettings::FIELD_DHCP_ENABLED   = "dhcp-enabled";
const std::string NetworkSettings::FIELD_STATIC_IP      = "static-ip";
const std::string NetworkSettings::FIELD_STATIC_MASK    = "static-mask";
const std::string NetworkSettings::FIELD_STATIC_GATEWAY = "static-gateway";

const std::map<NetworkSettings::Interface, std::vector<std::string>>& NetworkSettings::InterfaceValue::designators() const 
{ 
	static const std::map<Interface, std::vector<std::string>> result = 
	{
		{ Interface::ethernet, { "ethernet" } },
		{ Interface::wifi,     { "wifi"     } },
		
	};
			 
	return result; 
}		

NetworkSettings::NetworkSettings()
	: SettingsGroupBase()
{
	{
		auto& item = addItem<InterfaceValue>(FIELD_INTERFACE);
		item.setCaption("Network type");
		item.setIsRequired(true);
	}

	{
		auto& item = addItem<StringSettingValue>(FIELD_SSDP_NAME);
		item.setCaption("Discovery name");
		item.setIsRequired(false);
	}
	
	{
		auto& item = addItem<StringSettingValue>(FIELD_HOST);
		item.setCaption("Host");
		item.setIsRequired(false);
	}
	
	{
		auto& item = addItem<BooleanSettingValue>(FIELD_DHCP_ENABLED);
		item.setCaption("Is DHCP enabled");
		item.setDefaultValue(true);
		item.setIsRequired(false);
	}
	
	{
		auto& item = addItem<StringSettingValue>(FIELD_STATIC_IP);
		item.setCaption("Static IP");
		item.setIsRequired(false);
	}
	
	{
		auto& item = addItem<StringSettingValue>(FIELD_STATIC_MASK);
		item.setCaption("Mask");
		item.setIsRequired(false);
	}
	
	{
		auto& item = addItem<StringSettingValue>(FIELD_STATIC_GATEWAY);
		item.setCaption("Gateway");
		item.setIsRequired(false);
	}	
}

SettingValueSource NetworkSettings::getInterface(Interface& result_out) const
{
	const auto result = item<InterfaceValue>(FIELD_INTERFACE).get(result_out);
	return result;
}

void NetworkSettings::setInterface(Interface value)
{
	item<InterfaceValue>(FIELD_INTERFACE).set(value);
}

SettingValueSource NetworkSettings::getSsdpName(std::string& result_out) const
{
	const auto result = item<StringSettingValue>(FIELD_SSDP_NAME).get(result_out);
	return result;
}

void NetworkSettings::setSsdpName(const std::string& value)
{
	item<StringSettingValue>(FIELD_SSDP_NAME).set(value);
}

SettingValueSource NetworkSettings::getHost(std::string& result_out) const
{
	const auto result = item<StringSettingValue>(FIELD_HOST).get(result_out);
	return result;
}

void NetworkSettings::setHost(const std::string& value)
{
	item<StringSettingValue>(FIELD_HOST).set(value);
}

SettingValueSource NetworkSettings::getDhcpEnabled(bool& result_out) const
{
	const auto result = item<BooleanSettingValue>(FIELD_DHCP_ENABLED).get(result_out);
	return result;
}

void NetworkSettings::setDhcpEnabled(bool value)
{
	item<BooleanSettingValue>(FIELD_DHCP_ENABLED).set(value);
}

SettingValueSource NetworkSettings::getStaticIp(std::string& result_out) const
{
	const auto result = item<StringSettingValue>(FIELD_STATIC_IP).get(result_out);
	return result;
}

void NetworkSettings::setStaticIp(const std::string& value)
{
	item<StringSettingValue>(FIELD_STATIC_IP).set(value);
}

SettingValueSource NetworkSettings::getStaticMask(std::string& result_out) const
{
	const auto result = item<StringSettingValue>(FIELD_STATIC_MASK).get(result_out);
	return result;
}

void NetworkSettings::setStaticMask(const std::string& value)
{
	item<StringSettingValue>(FIELD_STATIC_MASK).set(value);
}

SettingValueSource NetworkSettings::getStaticGateway(std::string& result_out) const
{
	const auto result = item<StringSettingValue>(FIELD_STATIC_GATEWAY).get(result_out);
	return result;
}

void NetworkSettings::setStaticGateway(const std::string& value)
{
	item<StringSettingValue>(FIELD_STATIC_GATEWAY).set(value);
}