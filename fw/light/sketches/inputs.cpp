#include <Arduino.h>

#include "string_utils.h"
#include "inputs.h"

const uint8_t InputDevice::_expanderAddresses[] = 
{
	0x20, 0x21	
};

const std::vector<InputDevice::ChannelHw> InputDevice::_channelMap = 
{
	{ 0, 5 }, // 1
	{ 1, 3 }, // 2
	{ 0, 4 }, // 3
	{ 1, 4 }, // 4
	{ 0, 0 }, // 5
	{ 1, 0 }, // 6
	{ 0, 3 }, // 7
	{ 1, 5 }, // 8
	{ 0, 2 }, // 9
	{ 1, 6 }, // 10 
	{ 0, 1 }, // 11
	{ 1, 7 }, // 12
	{ 0, 7 }, // 13
	{ 1, 1 }, // 14
	{ 0, 6 }, // 15
	{ 1, 2 }, // 16
};

InputDevice::InputDevice() :
	IInputDevice()
{	
}

bool InputDevice::getCurrentValue(size_t channelIndex, DiscreteState& curValue_out) const
{
	const jm_PCF8574* device_p;	
	size_t channelNumber;
	
	if (!getHw(channelIndex, device_p, channelNumber))
	{
		return false;
	}
	
	if (!device_p->connected())
	{
		return false;
	}
	
	const auto state = device_p->digital_Read(channelNumber);
	curValue_out = state == HIGH ? DiscreteState::on : DiscreteState::off;
	
	return true;
}

void InputDevice::update()
{
	for (size_t i = 0; i < _expanderCount; ++i)
	{
		_expanders[i].readAll();
	}
}

bool InputDevice::getHw(size_t channelIndex, const jm_PCF8574*& device_p_out, size_t& channelNumber_out) const
{
	if (channelIndex >= _channelMap.size())
	{
		return false;
	}
	
	const auto& item = _channelMap[channelIndex];
	
	device_p_out = &_expanders[item.expanderIndex];
	channelNumber_out = item.channelNumber;
	
	return true;
}


bool InputDevice::initialize()
{
	for (const auto& hw : _channelMap)
	{
		auto& expander = _expanders[hw.expanderIndex];		
		expander.pin_Mode(hw.channelNumber, INPUT);
	}
	
	auto ok = false;
	
	for (size_t i = 0; i < _expanderCount; ++i)
	{
		auto& expander = _expanders[i];
		const auto i2cAddress = _expanderAddresses[i];
		
		expander.setIsBuffered(true);
		if (!expander.begin(i2cAddress))
		{
			LogError msg(_logger, ILogger::ErrorSeverity::warning);
			
			msg.addText("Failed to initialize input I2C device at address ");
			msg.addText(StringUtils::toString(i2cAddress));
		}
		else
		{
			ok = true;
		}
	}
	
	return ok;
}
