#include <Arduino.h>

#include "string_utils.h"
#include "outputs.h"

const uint8_t OutputDevice::_expanderAddresses[] = 
{
	0x24
};

const std::vector<OutputDevice::ChannelHw> OutputDevice::_channelMap = 
{
	{ 0, 5 },  // 1
	{ 0, 4 },  // 2
	{ 0, 0 },  // 3
	{ 0, 3 },  // 4
	{ 0, 2 },  // 5
	{ 0, 1 },  // 6
	{ 0, 7 },  // 7
	{ 0, 6 }   // 8	
};

OutputDevice::OutputDevice() : 
	IOutputDevice()
{	
}

void OutputDevice::setCurrentValue(size_t channelIndex, DiscreteState curValue_out)
{
	jm_PCF8574* device_p;	
	size_t channelNumber;
	
	if (!getHw(channelIndex, device_p, channelNumber))
	{
		return;
	}
	
	if (!device_p->connected())
	{
		return;
	}
	
	const auto state = curValue_out == DiscreteState::on ? LOW : HIGH;
	device_p->digital_Write(channelNumber, state);
}

void OutputDevice::update()
{
	for (size_t i = 0; i < _expanderCount; ++i)
	{
		_expanders[i].writeAll();
	}
}

bool OutputDevice::getHw(size_t channelIndex, jm_PCF8574 * & device_p_out, size_t& channelNumber_out)
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

bool OutputDevice::initialize()
{	
	for (const auto& hw : _channelMap)
	{
		auto& expander = _expanders[hw.expanderIndex];		
		expander.pin_Mode(hw.channelNumber, OUTPUT);
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
