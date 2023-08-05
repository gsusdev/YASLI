#include "settings_mqtt.h"

const std::string MqttSettings::FIELD_ENABLED = "enabled";
const std::string MqttSettings::FIELD_USER = "user";
const std::string MqttSettings::FIELD_PASSWORD = "password";
const std::string MqttSettings::FIELD_HOST = "host";
const std::string MqttSettings::FIELD_PORT = "port";
const std::string MqttSettings::FIELD_INPUT_TOPIC = "input-topic";
const std::string MqttSettings::FIELD_OUTPUT_TOPIC = "output-topic";
const std::string MqttSettings::FIELD_IDENTIFIER = "id";

MqttSettings::MqttSettings()
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
	 
	auto& host = addItem<StringSettingValue>(FIELD_HOST);
	host.setCaption("Host");
	host.setIsRequired(true);
	
	auto& port = addItem<IntegerSettingValue>(FIELD_PORT);
	port.setCaption("Port");
	port.setDefaultValue(0);
	port.setIsRequired(true);
	
	auto& id = addItem<StringSettingValue>(FIELD_IDENTIFIER);
	id.setCaption("Id");
	id.setIsRequired(false);
	
	auto& inputTopic = addItem<StringSettingValue>(FIELD_INPUT_TOPIC);
	inputTopic.setCaption("Id");
	inputTopic.setIsRequired(false);
	
	auto& outputTopic = addItem<StringSettingValue>(FIELD_OUTPUT_TOPIC);
	outputTopic.setCaption("Id");
	outputTopic.setIsRequired(false);
}

SettingValueSource MqttSettings::getUser(std::string& result_out) const
{
	const auto result = item<StringSettingValue>(FIELD_USER).get(result_out);
	return result;
}

void MqttSettings::setUser(const std::string& value)
{
	item<StringSettingValue>(FIELD_USER).set(value);
}

SettingValueSource MqttSettings::getPassword(std::string& result_out) const
{
	const auto result = item<StringSettingValue>(FIELD_PASSWORD).get(result_out);
	return result;
}

void MqttSettings::setPassword(const std::string& value)
{
	item<StringSettingValue>(FIELD_PASSWORD).set(value);
}
	
SettingValueSource MqttSettings::getHost(std::string& result_out) const
{
	const auto result = item<StringSettingValue>(FIELD_HOST).get(result_out);
	return result;
}

void MqttSettings::setHost(const std::string& value)
{
	item<StringSettingValue>(FIELD_HOST).set(value);
}

SettingValueSource MqttSettings::getPort(int& result_out) const
{
	const auto result = item<IntegerSettingValue>(FIELD_PORT).get(result_out);
	return result;
}

void MqttSettings::setPort(int value)
{
	item<IntegerSettingValue>(FIELD_PORT).set(value);
}

SettingValueSource MqttSettings::getEnabled(bool& result_out) const
{
	const auto result = item<BooleanSettingValue>(FIELD_ENABLED).get(result_out);
	return result;
}

void MqttSettings::setEnabled(bool value)
{
	item<BooleanSettingValue>(FIELD_ENABLED).set(value);
}

SettingValueSource MqttSettings::getInputControlTopic(std::string& result_out) const
{
	const auto result = item<StringSettingValue>(FIELD_INPUT_TOPIC).get(result_out);
	return result;
}

void MqttSettings::setInputControlTopic(const std::string& value)
{
	item<StringSettingValue>(FIELD_INPUT_TOPIC).set(value);
}

SettingValueSource MqttSettings::getOutputControlTopic(std::string& result_out) const
{
	const auto result = item<StringSettingValue>(FIELD_INPUT_TOPIC).get(result_out);
	return result;
}

void MqttSettings::setOutputControlTopic(const std::string& value)
{
	item<StringSettingValue>(FIELD_INPUT_TOPIC).set(value);
}

SettingValueSource MqttSettings::getIdentifier(std::string& result_out) const
{
	const auto result = item<StringSettingValue>(FIELD_IDENTIFIER).get(result_out);
	return result;
}

void MqttSettings::setIdentifier(const std::string& value)
{
	item<StringSettingValue>(FIELD_IDENTIFIER).set(value);
}
