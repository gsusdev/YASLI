#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdbool>

#include <vector>

#include "jm_PCF8574.h"

#include "types.h"
#include "log_utils.h"

class InputDevice : public IInputDevice
{
public: 
	InputDevice();
	
	void setLogger(ILogger* value_p) { _logger.setLogger(value_p); }
	
	bool initialize();
	
	bool getCurrentValue(size_t channelIndex, DiscreteState& curValue_out) const override;
	void update();
	
	size_t getChannelCount() const { return _channelMap.size(); }
	
private:
	bool getHw(size_t channelIndex, const jm_PCF8574 * & device_p_out, size_t& channelNumber_out) const;
	
	static const size_t _expanderCount = 2;
	
	LoggerHelper _logger;
	
	jm_PCF8574 _expanders[_expanderCount];
	
	struct ChannelHw
	{
		size_t expanderIndex;
		size_t channelNumber;
	};	
	
	static const std::vector<ChannelHw> _channelMap;
	static const uint8_t _expanderAddresses[_expanderCount];
};