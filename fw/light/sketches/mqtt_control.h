#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdbool>

#include <string>
#include <vector>
#include <sstream> 

#include <functional>

#include "AsyncMqttClient.h"

#include "types.h"
#include "log_utils.h"

class MqttControl : public IInputEventListener
{
public:
	MqttControl();
	~MqttControl();
	
	void setLogger(ILogger* value_p) { _logger.setLogger(value_p); }
	void setLightController(ILightControllerFacade* value_p);
	
	const std::string& getHost() const { return _host; }
	void setHost(const std::string& value) { _host = value; }
	
	int getPort() const { return _port; }
	void setPort(int value) { _port = value; }
	
	const std::string& getUser() const { return _user; }
	void setUser(const std::string& value) { _user = value; }
	
	const std::string& getPassword() const { return _password; }
	void setPassword(const std::string& value) { _password = value; }
		
	const std::string& getInputControlTopic() const { return _inputControlTopic; }
	void setInputControlTopic(const std::string& value) { _inputControlTopic = value; }
	
	const std::string& getOutputControlTopic() const { return _outputControlTopic; }
	void setOutputControlTopic(const std::string& value) { _outputControlTopic = value; }
	
	const std::string& getIdentifier() const { return _identifier; }
	void setIdentifier(const std::string& value) { _identifier = value; }	
	
	const std::string& getIpAddress() const { return _ipAddress; }
	void setIpAddress(const std::string& value) { _ipAddress = value; }	
	
	size_t getTimestamp() const { return _timestamp; }
	void setTimestamp(size_t value) { _timestamp = value; }
	
	size_t getMaxEventCount() const { return _maxEventCount; }
	void setMaxEventCount(size_t value) { _maxEventCount = value; }
	
	size_t getMaxMessageSize() const { return _maxMessageSize; }
	void setMaxMessageSize(size_t value) { _maxMessageSize = value; }
	
	void sendEvents();
	
	bool connect();
	void disconnect();
	
	bool isConnected() const { return _state == State::connected; }
	
private:	
	struct InputEventInfo
	{
		size_t channelIndex;
		EventType event;
	};
	
	enum class State
	{
		disconnected,
		connecting,
		connected,
		disconnecting
	};
	
using InputMessages = std::vector<InputEventInfo>;
	
	void prepareMessage(std::ostream& stream) const;
	
	void inputEventNotification(size_t channelIndex, EventType event) override;
	
	void onMqttConnect(bool sessionPresent);
	void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
	void onMqttSubscribe(uint16_t packetId, uint8_t qos);
	void onMqttUnsubscribe(uint16_t packetId);
	void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
	void onMqttPublish(uint16_t packetId);
		
	mutable LoggerHelper _logger;
	ILightControllerFacade* _lightController_p = nullptr;
	
	size_t _maxEventCount = 16;
	size_t _maxMessageSize = 1024;
	
	std::string _host;
	int _port;
	std::string _user;
	std::string _password;
	std::string _inputControlTopic;
	std::string _outputControlTopic;
	std::string _identifier;
	
	State _state = State::disconnected;
	
	std::string _ipAddress;
	uint32_t _timestamp = 0;
	
	AsyncMqttClient _mqttClient;
	
	InputMessages _inputEvents;
	
	std::stringstream _stringStream;
};