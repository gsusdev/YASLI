#include <string>
#include <functional>
#include <limits>

#include <Arduino.h>
#include <Wire.h>

#include <SPIFFS.h>

//#include <WiFi.h>
#include <ETH.h>

#include <WebServer.h>

#include "ESP8266FtpServer.h"
#include "ESP32SSDP.h"

#include "AsyncMqttClient.h"

#include "IniFile.h"

#include "string_utils.h"

#include "log_writer.h"
#include "logger.h"

#include "default_settings.h"
#include "settings_reader_ini.h"
#include "settings_network.h"
#include "settings_wifi.h"
#include "settings_ftp.h"
#include "settings_mqtt.h"

#include "main_stuff.h"
#include "http_control.h"
#include "mqtt_control.h"
#include <ETH.h>

static const String mainSettingsFilename = "/settings.ini";


#define PIN_I2C_SDA 4
#define PIN_I2C_SCL 5

#define PIN_ETHERNET_ENABLE 16
#define ETH_MDIO_PIN                       18
#define ETH_TXD0_PIN                       19
#define ETH_TXEN_PIN                       21
#define ETH_TXD1_PIN                       22
#define ETH_MDC_PIN                        23
#define ETH_RXD0_PIN                       25
#define ETH_RXD1_PIN                       26
#define ETH_MODE2_PIN                      27
#define ETH_POWER_PIN                      16
#define ETH_ADDR                            0
#define ETH_TYPE              ETH_PHY_LAN8720
#define ETH_CLK_MODE     ETH_CLOCK_GPIO17_OUT


static Logger logger;
static StreamLogWriter logSerialWriter;

static DefaultSettingsCreator defSettingsCreator;
static SettingsReaderIni settingsReader;

static WifiSettings wifiSettings;

static NetworkSettings networkSettings;

static FtpSettings ftpSettings;

static MqttSettings mqttSettings;

static FtpServer ftpServer;

static WebServer httpServer(80);

static LightControllerFacade lightController;
static HttpControl httpControl;
static MqttControl mqttControl;

static bool eth_connected = false;
static bool mqtt_enabled = false;

void WiFiEvent(WiFiEvent_t event)
{
	std::string hostName;
	networkSettings.getHost(hostName);
	
	switch (event) {
		case SYSTEM_EVENT_ETH_START:
		Serial.println("ETH Started");
		ETH.setHostname(hostName.c_str());
		break;
		case SYSTEM_EVENT_ETH_CONNECTED:
		Serial.println("ETH Connected");
		if (mqtt_enabled)
		{
			const std::string curIpAddress = ETH.localIP().toString().c_str();
			mqttControl.setIpAddress(curIpAddress);
			mqttControl.connect();
		}
		break;
		case SYSTEM_EVENT_ETH_GOT_IP:
		Serial.print("ETH MAC: ");		
		Serial.print(ETH.macAddress());
		Serial.print(", IPv4: ");
		Serial.print(ETH.localIP());
		if (ETH.fullDuplex()) {
			Serial.print(", FULL_DUPLEX");
		}
		Serial.print(", ");
		Serial.print(ETH.linkSpeed());
		Serial.println("Mbps");
		eth_connected = true;
		break;
		case SYSTEM_EVENT_ETH_DISCONNECTED:
		Serial.println("ETH Disconnected");
		eth_connected = false;
		break;
		case SYSTEM_EVENT_ETH_STOP:
		Serial.println("ETH Stopped");
		eth_connected = false;
		break;
		default:
		break;
	}
}

bool initEthernet(const NetworkSettings& networkSettings)
{
	//digitalWrite(PIN_ETHERNET_ENABLE, HIGH);
	
	WiFi.onEvent(WiFiEvent);
		
	const auto ok = ETH.begin(ETH_ADDR, ETH_POWER_PIN, ETH_MDC_PIN, ETH_MDIO_PIN, ETH_TYPE, ETH_CLK_MODE);
	return ok;
}

bool initWiFi(const WifiSettings& settings)
{	
	/*WifiSettings::WifiRole role;
	std::string ssid;
	std::string password;
	
	auto paramsOk = true;
	
	if (!settingOk(settings.getRole(role)))
	{
		paramsOk = false;
		logger.error("Wifi role is not set", ILogger::ErrorSeverity::error);		
	}
	
	if (!settingOk(settings.getSid(ssid)))
	{
		paramsOk = false;
		logger.error("Wifi SSID is not set", ILogger::ErrorSeverity::error);		
	}
	
	if (!settingOk(settings.getPassword(password)))
	{
		paramsOk = false;
		logger.error("Wifi password is not set", ILogger::ErrorSeverity::error);		
	}
	
	if (!paramsOk)
	{
		return false;
	}
	
	if (role == WifiSettings::WifiRole::accessPoint)
	{
		logger.trace("Wifi mode: access point");
		
		if (!WiFi.softAP(ssid.c_str(), password.c_str()))
		{
			logger.error("Failed to initialize Wifi access point mode", ILogger::ErrorSeverity::error);
			return false;
		}
		
		const auto& ip = WiFi.softAPIP();
		
		LogInfo msg(logger);
		
		msg.addText("\nInitialized WiFi access point\n");
		msg.addText("\tSSID: " + ssid + "\n");
		msg.addText("\tPassword: " + password + "\n");
		msg.addText("\tIP address: " + std::string(ip.toString().c_str()) + "\n");
		msg.addText("\n");
		
	}
	else if (role == WifiSettings::WifiRole::station)
	{
		logger.trace("Wifi mode: station");
		
		if (!WiFi.begin(ssid.c_str(), password.c_str()))
		{
			logger.error("Failed to initialize Wifi", ILogger::ErrorSeverity::error);
			return false;
		}
		
		int timeout_s = 30;
		
		while (WiFi.status() != WL_CONNECTED) 
		{
			delay(1000);
			
			const auto status = WiFi.status();
			if (status == WL_CONNECTED)
			{
				break;
			}
			
			if (status == WL_NO_SSID_AVAIL)
			{
				logger.error("\nWiFi SSID: " + ssid + " not found" + "\n", ILogger::ErrorSeverity::error);
				return false;
			}
			
			if (status == WL_CONNECT_FAILED)
			{
				logger.error("\nFailed to connect to WiFi, SSID: " + ssid + "\n", ILogger::ErrorSeverity::error);
				return false;
			}
			
			logger.trace("Connecting to WiFi " + StringUtils::toString(timeout_s) + ", status: " + StringUtils::toString(status));
			
			if (--timeout_s <= 0)
			{
				break;
			}
		}
		
		if (WiFi.status() != WL_CONNECTED)
		{
			logger.error("\nWiFi connection timeout, SSID: " + ssid + "\n", ILogger::ErrorSeverity::error);
			return false;
		}
		else
		{
			const auto& ip = WiFi.localIP();
			
			LogInfo msg(logger);
		
			msg.addText("\nConnected to WiFi, SSID: " + ssid + "\n");
			msg.addText("\tIP address: " + std::string(ip.toString().c_str()) + "\n");
			msg.addText("\n");
		}
	}
	else
	{
		logger.error("Wifi mode is not supported", ILogger::ErrorSeverity::error);		
		return false;
	}*/
	
	return true;
}

bool initMqtt(const MqttSettings& settings, bool& isEnabled)
{
	bool enabled = false;
	
	std::string host;
	int port;
	std::string user;
	std::string password;
	std::string inputControlTopic;
	std::string outputControlTopic;
	std::string identifier;
	
	auto paramsOk = true;
	
	if (!settingOk(settings.getEnabled(enabled)))
	{
		paramsOk = false;
		enabled = false;
		logger.error("MQTT server enabled is not specified", ILogger::ErrorSeverity::error);		
	}
	
	if (enabled)
	{
		if (!settingOk(settings.getHost(host)) || host.empty())
		{
			logger.error("MQTT host is not specified", ILogger::ErrorSeverity::error);
			paramsOk = false;
		}
		
		if (!settingOk(settings.getPort(port)))
		{
			logger.error("MQTT port is not specified. Using 1833", ILogger::ErrorSeverity::warning);
			port = 1833;
		}
		
		if (!settingOk(settings.getUser(user)) || user.empty())
		{
			logger.error("MQTT user is not specified", ILogger::ErrorSeverity::warning);
		}
		
		if (!settingOk(settings.getPassword(password)) || password.empty())
		{
			logger.error("MQTT password is not specified", ILogger::ErrorSeverity::warning);
		}
		
		if (!settingOk(settings.getInputControlTopic(inputControlTopic)) || inputControlTopic.empty())
		{
			logger.error("MQTT input control topic is not specified", ILogger::ErrorSeverity::warning);			
		}
		
		if (!settingOk(settings.getOutputControlTopic(outputControlTopic)) || outputControlTopic.empty())
		{
			logger.error("MQTT output control topic is not specified", ILogger::ErrorSeverity::warning);
		}
		
		if (!settingOk(settings.getIdentifier(identifier)) || identifier.empty())
		{
			logger.error("MQTT identifier is not specified", ILogger::ErrorSeverity::warning);
		}
		
		if (paramsOk)
		{
			mqttControl.setLightController(&lightController);
			mqttControl.setLogger(&logger);
			mqttControl.setHost(host);
			mqttControl.setPort(port);
			mqttControl.setUser(user);
			mqttControl.setPassword(password);
			mqttControl.setIdentifier(identifier);
			mqttControl.setInputControlTopic(inputControlTopic);
			mqttControl.setOutputControlTopic(outputControlTopic);	
		}
	}
	
	isEnabled = enabled;
	return paramsOk;
}

bool initFtp(const FtpSettings& settings)
{
	bool enabled;
	std::string user;
	std::string password;
	int port;
	
	auto paramsOk = true;
	
	if (!settingOk(settings.getEnabled(enabled)))
	{
		paramsOk = false;
		enabled = false;
		logger.error("FTP server enabled is not specified", ILogger::ErrorSeverity::error);		
	}
	
	if (enabled)
	{
		if (!settingOk(settings.getUser(user)) || user.empty())
		{
			logger.error("FTP server user is not specified", ILogger::ErrorSeverity::warning);
		}
		
		if (!settingOk(settings.getPassword(password)) || password.empty())
		{
			logger.error("FTP server password is not specified", ILogger::ErrorSeverity::warning);
		}
		
		if (settingOk(settings.getPort(port)) || (port != 21))
		{
			logger.error("FTP server port is forced to be 21", ILogger::ErrorSeverity::warning);
			port = 21;
		}
	}
	else
	{
		if (paramsOk)
		{
			logger.info("FTP server is disabled");
		}
	}
	
	if (!paramsOk || !enabled)
	{
		return false;
	}
	
	ftpServer.setFileSystem(&SPIFFS);
	ftpServer.begin(String(user.c_str()), String(password.c_str()));
	
	return true;
}

bool initSsdp(WebServer& srv, const std::string& name)
{
	srv.on("/description.xml",
		HTTP_GET,
		[&srv]() {
			SSDP.schema(srv.client());			
		});
		
	SSDP.setSchemaURL("description.xml");
	SSDP.setHTTPPort(80);
	SSDP.setName(name.c_str());
	SSDP.setSerialNumber("0001");
	SSDP.setURL("index.html");
	SSDP.setModelName("YASLI Simple Combo");
	SSDP.setModelDescription("Discrete 16 inputs 8 outputs controller ");
	SSDP.setModelNumber("1");
	SSDP.setModelURL("http://.com");
	SSDP.setManufacturer("GSUS");
	SSDP.setManufacturerURL("http://.com");
	//"urn:schemas-upnp-org:device:Basic:1" if not set
	SSDP.setDeviceType("upnp:rootdevice");  //to appear as root device
	SSDP.setServerName("SSDPServer/1.0");
	//set UUID, you can use https://www.uuidgenerator.net/
	//use 38323636-4558-4dda-9188-cda0e6 + 4 last bytes of mac address if not set
	//use SSDP.setUUID("daa26fa3-d2d4-4072-bc7a-a1b88ab4234a", false); for full UUID
	//SSDP.setUUID("d85ca788-5ec7-11eb-ae93-0242ac130002");
	//Set icons list, NB: optional, this is ignored under windows
	SSDP.setIcons("<icon>"
	                "<mimetype>image/png</mimetype>"
	                "<height>48</height>"
	                "<width>48</width>"
	                "<depth>24</depth>"
	                "<url>icon48.png</url>"
	                "</icon>");
	//Set service list, NB: optional for simple device
	SSDP.setServices("<service>"
	                   "<serviceType>urn:schemas-upnp-org:service:SwitchPower:1</serviceType>"
	                   "<serviceId>urn:upnp-org:serviceId:SwitchPower:1</serviceId>"
	                   "<SCPDURL>/SwitchPower1.xml</SCPDURL>"
	                   "<controlURL>/SwitchPower/Control</controlURL>"
	                   "<eventSubURL>/SwitchPower/Event</eventSubURL>"
	                   "</service>");

	const auto ok = SSDP.begin();
	return ok;
}

bool initHttpControl(WebServer& srv)
{	
	httpControl.setLightController(&lightController);
	httpControl.setLogger(&logger);
	
	srv.on("/api/v1/inputs/get", HTTP_GET, 
		[&srv]() {
			httpControl.inputs_state_get(srv);	
		});
	
	srv.on("/api/v1/outputs/get",
		HTTP_GET, 
		[&srv]() {
			httpControl.outputs_state_get(srv);	
		});
	
	srv.on("/api/v1/outputs/set",
		HTTP_GET, 
		[&srv]() {
			httpControl.outputs_state_set(srv);	
		});
	
	srv.on("/api/v1/outputs/all-off",
		HTTP_GET, 
		[&srv]() {
			httpControl.outputs_local_off(srv);	
		});
		
	return true;
}

void setup()
{
	//pinMode(PIN_ETHERNET_ENABLE, OUTPUT);
	Serial.begin(115200);
		
	logSerialWriter.setStream(&Serial);
	logger.setWriter(&logSerialWriter);
	
	logger.trace("Initialization");
	
	if (!Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL, 100000))
	{
		logger.error("I2C initialization failed", ILogger::ErrorSeverity::error);
	}
	
	auto areSettingsEmpty = false;
	logger.trace("Initializing file system");	
	if (!SPIFFS.begin(false))
	{		
		logger.error("Initializing file system failed. Trying to format", ILogger::ErrorSeverity::error);
		if (!SPIFFS.begin(true))
		{		
			logger.error("SPIFFS initialization failed", ILogger::ErrorSeverity::critical);	
			abort();
		}
		
		areSettingsEmpty = true;
		logger.trace("Initializing main settings defaults");
	
		defSettingsCreator.setLogger(&logger);
		defSettingsCreator.setFileSystem(&SPIFFS);
		defSettingsCreator.setMainSettingsFilename(mainSettingsFilename);
		defSettingsCreator.setOverwriteIfExists(true);
		if (!defSettingsCreator.execute())
		{
			logger.error("Default main settings creation failed", ILogger::ErrorSeverity::critical);
			abort();
		}
	}
	
	
	
	logger.trace("Reading settings");	
	
	settingsReader.setFileSystem(&SPIFFS);
	settingsReader.setLogger(&logger);
	if (!settingsReader.open(mainSettingsFilename))
	{
		logger.error("Failed to open settings file", ILogger::ErrorSeverity::critical);	
		abort();
	}
	
	if (settingsReader.readGroup("network", networkSettings))
	{
		NetworkSettings::Interface netIntf;
		if (settingOk(networkSettings.getInterface(netIntf)))
		{
			if (netIntf == NetworkSettings::Interface::wifi)
			{
				if (settingsReader.readGroup("wifi", wifiSettings))
				{
					logger.trace("Initializing Wifi");
		
					if (!initWiFi(wifiSettings))
					{
						logger.error("Wifi initialization failed", ILogger::ErrorSeverity::error);	
					}
				}
				else
				{
					logger.error("Failed to read Wifi settings", ILogger::ErrorSeverity::error);	
				}
			}
			else if (netIntf == NetworkSettings::Interface::ethernet)
			{
				if (!initEthernet(networkSettings))
				{
					logger.error("Failed to initialize ethernet", ILogger::ErrorSeverity::error);	
				}
			}
			else
			{
				logger.error("Network interface is not supported", ILogger::ErrorSeverity::error);	
			}
		}
		else
		{
			logger.error("Network interface is not specified", ILogger::ErrorSeverity::error);	
		}
	}
	else
	{
		logger.error("Failed to read network settings", ILogger::ErrorSeverity::error);	
	}
	
	
	if (settingsReader.readGroup("ftp", ftpSettings))
	{
		logger.trace("Initializing FTP server");
		
		if (!initFtp(ftpSettings))
		{
			logger.error("FTP server initialization failed", ILogger::ErrorSeverity::error);	
		}
	}
	else
	{
		logger.error("Failed to read FTP server settings", ILogger::ErrorSeverity::error);	
	}
	
	if (settingsReader.readGroup("mqtt", mqttSettings))
	{
		logger.trace("Initializing MQTT client");
		
		if (!initMqtt(mqttSettings, mqtt_enabled))
		{
			mqtt_enabled = false;
			logger.error("MQTT client initialization failed", ILogger::ErrorSeverity::error);	
		}
	}
	else
	{
		logger.error("Failed to read MQTT client settings", ILogger::ErrorSeverity::error);	
	}
	

	
	logger.trace("Initializing SSDP");
	std::string ssdpName;
	networkSettings.getSsdpName(ssdpName);
	if (!initSsdp(httpServer, ssdpName))
	{
		logger.error("SSDP initialization failed", ILogger::ErrorSeverity::error);
	}
	
	logger.trace("Initializing HTTP control");
	if (!initHttpControl(httpServer))
	{
		logger.error("HTTP control initialization failed", ILogger::ErrorSeverity::error);
	}
	
	logger.trace("Initializing HTTP server");
	httpServer.begin();
	
	if (!lightController.initialize(&logger, &SPIFFS, areSettingsEmpty))
	{
		logger.error("Failed to initialize light control system", ILogger::ErrorSeverity::error);	
	}
		
	
	logger.info("Initialization completed");
}

static unsigned long lastMillis = 0;
uint64_t cnt = 0;
void loop()
{		
	const auto curMillis = millis();
	cnt++;
	
	const int delay_msec = 5;
	
	unsigned long dif = curMillis - lastMillis;
	if (curMillis > lastMillis)
	{
		dif = curMillis - lastMillis;
	}
	else if (curMillis < lastMillis)
	{
		dif = curMillis + std::numeric_limits<unsigned long>::max() - lastMillis;
	}
	else
	{
		return;
	}	
		
	ftpServer.handleFTP();
	httpServer.handleClient();
	
	if (dif > delay_msec)
	{
		lightController.loop(dif);		
		mqttControl.setTimestamp(curMillis);
		
		if (mqttControl.isConnected())
		{
			mqttControl.sendEvents();
		}
		
		lastMillis = curMillis;	
		
	}
	
}

