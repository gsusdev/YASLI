#include <cassert>

#include "default_settings.h"

const String DefaultSettingsCreator::_mainSettingsText = 
R"text(
; Wifi settings
[wifi] 
	; Mode: 
	;	station      : this device connects to the router or something similar
	;	access-point : other device (smartphone etc) connects to this device	
	mode=station
	;mode=access-point
	
	; the identifier of the network to connect to
	ssid=temp
	
	; security password of the network
	password=1234567890
	
[network]
	; Primary network interface:
	;	ethernet :  ethernet port is used
	;	wifi     :  Wifi is used
	interface=ethernet
	
	; The name that will be displayed in the network neighbourhood
	ssdp-name=Room-Light
	
	; Network symbolic name
	host=light.room.smarthome.mydomain
	
	; IP address, mask and gateway will be obtained from the DHCP server (usually from the router)
	dhcp-enabled=true
	
	; IP address that will be used if DHCP is disabled
	static-ip=0.0.0.0
	
	; Subnet mask that will be used if DHCP is disabled
	static-mask=0.0.0.0
	
	; Gateway that will be used if DHCP is disabled
	static-gateway=0.0.0.0
	
; FTP server settings
[ftp]
	; Is FTP server enabled:
	;	true:  enabled
	;	false: disabled
	enabled=true
	
	; User name
	user=user
	
	; Password
	password=1234567890

; MQTT client settings
[mqtt]
	; Is MQTT client enabled:
	;	true:	enabled
	;	false:	disabled
	enabled=false
	
	; User name (optional)
	;user=user
	
	; Password (optional)
	;password=password
	
	; MQTT server address
	host=192.168.1.65
	
	; MQTT server port
	port=1883
	
	; Topic for publishing input buttons events
	input-topic=light/input-events
	
	; Topic for receiving output control commands
	output-topic=light/output-control
	
	; Identifier for event publishing output control commands filtering
	;id=

)text";
	
const String DefaultSettingsCreator::_inputsText = 
R"text(
// Input channels list
//
// Items must not contain any whitespace characters (whitespace itself, tabulator etc.).
// Empty lines are allowed.
// To disable a line, add two slashes (//) at the beginning.

// Example:
//     door_switch
//     bed_switch_left_chandelier
//     bed_switch_left_sconce
//     bed_switch_right_chandelier
//     bed_switch_right_sconce

in1
in2
in3
in4
in5
in6
in7
in8
in9
in10
in11
in12
in13
in14
in15
in16
)text";
	
const String DefaultSettingsCreator::_outputsText = 
R"text(
// Output channels list
//
// Items must not contain any whitespace characters (whitespace itself, tabulator etc.).
// Empty lines are allowed.
// To disable a line, add two slashes (//) at the beginning.

// Example:
//     chandelier
//     bed_sconce_left
//     bed_sconce_right

out1
out2
out3
out4
out5
out6
out7
out8
)text";

const String DefaultSettingsCreator::_rulesText = 
R"text(
// Rules settings file
//
// Rule syntax:  
//     INPUT_CHANNEL EVENT OUTPUT_CHANNEL ACTION
//
// Empty lines are allowed.
// To disable line, add two slashes (//) at the beginning.
//
// Events:  
//     fall - event is fired on switching input off
//     rise - event is fired on switching input on
//
// Actions: 
//     turn_off   - output is switched off
//     turn_on    - output is switched on
//     toggle     - output`s state is turned into an opposite one
//     local_off  - all outputs are turned off
//     global_off - all ouputs are turned off, a pulse is fetched to the global off output
//
// For the local_off and global_off actions OUTPUT_CHANNEL may be anything, but must not be ommited (will be fixed later maybe).
//
// Example: 
//     door_switch fall chandelier toggle
//     bed_switch_left_chandelier fall chandelier toggle
//     bed_switch_right_chandelier fall chandelier toggle
//     bed_switch_left_sconce fall bed_sconce_left toggle
//     bed_switch_right_sconce fall bed_sconce_right toggle
//
// This example shows a bedroom configuration when we have a chandelier and two sconces on the sides of the bed.
// Chanlelier is controlled by the switch near the door and two switches on the sides of the bed.
// Sconces are controlled by the related switches on the sides of the bed.
	
in1 fall out1 turn_off
in1 rise out1 turn_on

in2 fall out2 turn_off
in2 rise out2 turn_on
	
//in3 fall out1 turn_off
//in3 rise out1 turn_on
in3 fall out1 toggle

in4 fall out4 turn_off
in4 rise out4 turn_on

in5 fall out5 turn_off
in5 rise out5 turn_on

in6 fall out6 turn_off
in6 rise out6 turn_on

in7 fall out7 turn_off
in7 rise out7 turn_on

in8 fall out8 turn_off
in8 rise out8 turn_on

in9 fall out1 turn_off
in9 rise out1 turn_on

in10 fall out2 turn_off
in10 rise out2 turn_on

in11 fall out3 turn_off
in11 rise out3 turn_on

in12 fall out4 turn_off
in12 rise out4 turn_on

in13 fall out5 turn_off
in13 rise out5 turn_on

in14 fall out6 turn_off
in14 rise out6 turn_on

in15 fall out7 turn_off
in15 rise out7 turn_on

in16 fall out8 turn_off
in16 rise out8 turn_on
)text";
	
bool DefaultSettingsCreator::execute()
{
	assert(_fileSystem_p != nullptr);
	
	auto ok = true;
				
	if (!_inputsFilename.isEmpty())
	{
		_logger.trace("Writing input channels defaults");
		
		ok &= writeText(_inputsText, _inputsFilename, _overwriteIfExists);
	}
	
	if (!_outputsFilename.isEmpty())	
	{
		_logger.trace("Writing output channels defaults");
		
		ok &= writeText(_outputsText, _outputsFilename, _overwriteIfExists);
	}
	
	if (!_rulesFilename.isEmpty())
	{
		_logger.trace("Writing rules defaults");
		
		ok &= writeText(_rulesText, _rulesFilename, _overwriteIfExists);
	}	
	
	if (!_mainSettingsFilename.isEmpty())
	{
		_logger.trace("Writing main settings");
		
		ok &= writeText(_mainSettingsText, _mainSettingsFilename, _overwriteIfExists);
	}	
	
	return ok;
}


bool DefaultSettingsCreator::writeText(const String& text, const String& filename, bool overwriteIfExists)
{
	if (!overwriteIfExists && _fileSystem_p->exists(filename))
	{
		LogInfo msg(_logger);
		
		msg.addText("Skipping already existing settings file ");
		msg.addText(std::string(filename.c_str()));
		
		return true;
	}
	
	auto f = _fileSystem_p->open(filename, "w");
	if (!f)
	{
		LogInfo msg(_logger);
		
		msg.addText("Failed to write to file ");
		msg.addText(std::string(filename.c_str()));
		
		return false;
	}
	
	const auto sz = f.write(reinterpret_cast<const uint8_t*>(text.c_str()), text.length());
	const auto ok = sz == text.length();	
	f.close();
	
	return ok;
}
