#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdbool>

#include <string>
#include <map>

#include <functional>

#include <Arduino.h> 
#include <WebServer.h>

#include "types.h"
#include "log_utils.h"

class HttpControl
{
public:
	HttpControl();
	void setLogger(ILogger* value_p) { _logger.setLogger(value_p); }
	void setLightController(ILightControllerFacade* value_p) { _lightController_p = value_p; }
	
	void inputs_state_get(WebServer& srv) const;
	
	void outputs_state_get(WebServer& srv) const;
	void outputs_state_set(WebServer& srv);
	
	void outputs_local_off(WebServer& srv);
	
private:
	bool getStateList(const std::vector<std::string>& names, std::function<DiscreteState(const std::string* name_p)> stateGetter, String& result_out) const;
		
	mutable LoggerHelper _logger;
	ILightControllerFacade* _lightController_p = nullptr;
	
	mutable std::vector<std::string> _channelNames;
	mutable String _buf;
};