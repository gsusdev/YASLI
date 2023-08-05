#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdbool>

#include <vector>

#include "jm_PCF8574.h"

#include "types.h"
#include "log_utils.h"

class OutputDevice : public IOutputDevice
{
public:
	OutputDevice(); 
	
	void setLogger(ILogger* value_p) { _logger.setLogger(value_p); }
	
	bool initialize();
	
	void setCurrentValue(size_t channelIndex, DiscreteState value) override;
	void update();
	
	size_t getChannelCount() const { return _channelMap.size(); }
	
private:
	bool getHw(size_t channelIndex, jm_PCF8574 * & device_p_out, size_t& channelNumber_out);
	
	static const size_t _expanderCount = 1;
	
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