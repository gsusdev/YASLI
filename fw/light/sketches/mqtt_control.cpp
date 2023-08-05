#include "mqtt_control.h"

MqttControl::MqttControl()
{
	_inputEvents.reserve(16);
	
	using namespace std::placeholders;
	
	_mqttClient.onConnect    (std::bind(&MqttControl::onMqttConnect,     this, _1    ));
	_mqttClient.onDisconnect (std::bind(&MqttControl::onMqttDisconnect,  this, _1    ));
	_mqttClient.onSubscribe  (std::bind(&MqttControl::onMqttSubscribe,   this, _1, _2));
	_mqttClient.onUnsubscribe(std::bind(&MqttControl::onMqttUnsubscribe, this, _1    ));
	_mqttClient.onMessage    (std::bind(&MqttControl::onMqttMessage,     this, _1, _2, _3, _4, _5, _6));
	_mqttClient.onPublish    (std::bind(&MqttControl::onMqttPublish,     this, _1    ));	
}

MqttControl::~MqttControl()
{
	disconnect();
	setLightController(nullptr);
}

void MqttControl::setLightController(ILightControllerFacade* value_p) 
{  
	if (_lightController_p != nullptr) 
	{
		_lightController_p->unregisterInputEventListener(*this);
	}
		
	_lightController_p = value_p; 
	
	if (_lightController_p != nullptr)
	{
		_lightController_p->registerInputEventListener(*this);
	}
}

void MqttControl::inputEventNotification(size_t channelIndex, EventType event)
{
	if (_inputEvents.size() < _maxEventCount)
	{
		InputEventInfo info;
		
		info.channelIndex = channelIndex;
		info.event = event;
		 
		_inputEvents.push_back(info);
	}
}

void MqttControl::prepareMessage(std::ostream& stream) const
{
/* JSON message example
{
  "source-id": "light.bedroom",
  "source-ip": "192.168.1.6",
  "timestamp": 321,
  "events": [
      {
        "channel-index": 0,
        "event": "rise"
      },
      {
        "channel": 1,
        "event": "fall"
      }
    ]
}
*/
	stream << "{";
	
	if (!_identifier.empty())
	{
		stream << "\"source-id\":\"" << _identifier << "\",";
	}
	
	if (!_ipAddress.empty())
	{
		stream << "\"source-ip\":\"" << _ipAddress << "\",";
	}
	
	if (_timestamp != 0)
	{
		stream << "\"timestamp\":\"" << _timestamp << "\",";
	}
	
	stream << "\"events\":[";
	
	for (const auto& eventInfo : _inputEvents)
	{
		std::string sEvent;
		switch (eventInfo.event)
		{
			case EventType::rise:
			sEvent = "rise";
			break;
			
			case EventType::fall:
			sEvent = "fall";
			break;
			
			default:
			continue;
		}
		
		stream << "{";
		stream << "\"channel-index\":" << eventInfo.channelIndex << ",";
		stream << "\"event\":" << sEvent << ",";
		stream << "}";		
	}
	
	stream << "]}";
}

void MqttControl::sendEvents()
{	
	if (_inputEvents.empty())
	{
		return;
	}
	
	_stringStream.str(std::string());
	prepareMessage(_stringStream);
	_inputEvents.clear();
	
	_mqttClient.publish(_inputControlTopic.c_str(), 0, false, _stringStream.str().c_str());
	
	_logger.info("MqttControl: Event(s) published");
}

bool MqttControl::connect()
{
	if (_state != State::disconnected)
	{
		return false;
	}
	
	_mqttClient.setServer(_host.c_str(), _port);
	
	if (!_user.empty())
	{
		_mqttClient.setCredentials(_user.c_str(), _password.empty() ? nullptr : _password.c_str());
	}
	
	_mqttClient.connect();
	
	_state = State::connecting;
	return true;
}

void MqttControl::disconnect()
{
	if (_state == State::connected)
	{
		_mqttClient.disconnect(true);		
	}
	
	_state = State::disconnecting;
}

void MqttControl::onMqttConnect(bool sessionPresent) 
{
	_logger.info("MqttControl: Connected to server");
	
	if (_state == State::connecting)
	{
		_state = State::connected;
		
		if (!_outputControlTopic.empty())
		{		
			_logger.info("MqttControl: Subscripbing to topic");
			_mqttClient.subscribe(_outputControlTopic.c_str(), 2);
		}
	}
	else
	{
		_state = State::disconnecting;
		_mqttClient.disconnect(true);			
	}
}

void MqttControl::onMqttDisconnect(AsyncMqttClientDisconnectReason reason) 
{
	if (_state == State::connected)
	{
		_logger.error("MqttControl: Connection failed. Trying to reconnect", ILogger::ErrorSeverity::error);
		
		_state = State::connecting;
		_mqttClient.connect();
	}
	else if (_state == State::connecting)
	{
		std::string text;
		bool retry = false;
		
		switch (reason)
		{			
			case AsyncMqttClientDisconnectReason::TCP_DISCONNECTED:
			text = "TCP_DISCONNECTED";
			retry = true;
			break;

			case AsyncMqttClientDisconnectReason::MQTT_UNACCEPTABLE_PROTOCOL_VERSION:
			text = "MQTT_UNACCEPTABLE_PROTOCOL_VERSION";
			break;
			
			case AsyncMqttClientDisconnectReason::MQTT_IDENTIFIER_REJECTED:
			text = "MQTT_IDENTIFIER_REJECTED";
			break;
			
			case AsyncMqttClientDisconnectReason::MQTT_SERVER_UNAVAILABLE:
			text = "MQTT_SERVER_UNAVAILABLE";
			retry = true;
			break;
			
			case AsyncMqttClientDisconnectReason::MQTT_MALFORMED_CREDENTIALS:
			text = "MQTT_MALFORMED_CREDENTIALS";
			break;
			
			case AsyncMqttClientDisconnectReason::MQTT_NOT_AUTHORIZED:
			text = "MQTT_NOT_AUTHORIZED";
			break;

			case AsyncMqttClientDisconnectReason::ESP8266_NOT_ENOUGH_SPACE:
			text = "ESP8266_NOT_ENOUGH_SPACE";
			break;

			case AsyncMqttClientDisconnectReason::TLS_BAD_FINGERPRINT:
			text = "TLS_BAD_FINGERPRINT";
			break;		

			default:
				text = "Unknown error";
		}
		
		if (retry)
		{
			text += ". Retrying";
		}
		
		_logger.error("MqttControl: " + text, ILogger::ErrorSeverity::error);
		
		if (retry)
		{
			_state = State::connecting;
			_mqttClient.connect();
		}
		else
		{
			_state = State::disconnected;
		}
	}
	else
	{
		_state = State::disconnected;
		_logger.info("MqttControl: Disconnected from server");		
	}	
}

void MqttControl::onMqttSubscribe(uint16_t packetId, uint8_t qos) 
{
	_logger.info("MqttControl: Subscribe acknowledged");	
}

void MqttControl::onMqttUnsubscribe(uint16_t packetId) 
{
	
}

void MqttControl::onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
	/*Serial.println("Publish received.");
	Serial.print("  topic: ");
	Serial.println(topic);
	Serial.print("  qos: ");
	Serial.println(properties.qos);
	Serial.print("  dup: ");
	Serial.println(properties.dup);
	Serial.print("  retain: ");
	Serial.println(properties.retain);
	Serial.print("  len: ");
	Serial.println(len);
	Serial.print("  index: ");
	Serial.println(index);
	Serial.print("  total: ");
	Serial.println(total);*/
}

void MqttControl::onMqttPublish(uint16_t packetId) {
	_logger.info("MqttControl: Publish acknowledged");
}