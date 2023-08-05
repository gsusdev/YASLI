#include <cassert>

#include "http_control.h"

#define HTTP_OK 200
#define HTTP_BAD_REQUEST 400
#define HTTP_INTERNAL_SERVER_ERROR 500

bool HttpControl::getStateList(const std::vector<std::string>& names, std::function<DiscreteState(const std::string* name_p)> stateGetter, String& result_out) const
{
	assert(_lightController_p != nullptr);
	 
	auto ok = true;
	result_out.concat("[\n");
	
	for (auto it = names.begin(); it != names.end(); ++it)
	{
		const auto value = stateGetter(&*it);
			
		if (it != names.begin())
		{
			ok &= result_out.concat(", ");
		}
		
		ok &= result_out.concat("\t[ \"");
		
		ok &= result_out.concat(it->c_str());
		ok &= result_out.concat("\", ");
		
		switch (value)
		{
			case DiscreteState::unknown:
			ok &= result_out.concat("null");
			break;
			
			case DiscreteState::on:
			ok &= result_out.concat("true");
			break;
			
			case DiscreteState::off:
			ok &= result_out.concat("false");
			break;
			
			default:
				ok &= result_out.concat("null");
				_logger.error("HttpControl: Invalid DiscreteState value", ILogger::ErrorSeverity::error);
		}
		
		ok &= result_out.concat(" ]\n");
	}
	
	ok &= result_out.concat("\n]");
	return ok;	
}

void HttpControl::inputs_state_get(WebServer& srv) const
{
	assert(_lightController_p != nullptr);
	
	_channelNames.clear();
	
	const auto channelCount = _lightController_p->getInputCount();
	if (_channelNames.capacity() < channelCount)
	{
		_channelNames.reserve(channelCount);
	}
	
	_lightController_p->getInputNames(
		[this](const std::string* name_p)
		{
			this->_channelNames.push_back(*name_p);
		}
	);
	
	_buf = "";
	auto ok = getStateList(
		_channelNames, 
		[this](const std::string* name_p)
		{
			const auto result = this->_lightController_p->getInputState(*name_p);
			return result;
		},
		_buf			
	);
	
	if (!ok)
	{
		_logger.error("HttpControl: out of memory", ILogger::ErrorSeverity::error);
		srv.send(HTTP_INTERNAL_SERVER_ERROR, "text/plain", "Out of memory");
		return;
	}
	
	srv.send(HTTP_OK, "text/json", _buf);
}
	
void HttpControl::outputs_state_get(WebServer& srv) const
{
	assert(_lightController_p != nullptr);
	
	assert(_lightController_p != nullptr);
	
	_channelNames.clear();
	
	const auto channelCount = _lightController_p->getOutputCount();
	if (_channelNames.capacity() < channelCount)
	{
		_channelNames.reserve(channelCount);
	}
	
	_lightController_p->getOutputNames(
		[this](const std::string* name_p)
		{
			this->_channelNames.push_back(*name_p);
		}
	);
	
	_buf = "";
	auto ok = getStateList(
		_channelNames, 
		[this](const std::string* name_p)
		{
			const auto result = this->_lightController_p->getOutputState(*name_p);
			return result;
		},
		_buf);
	
	if (!ok)
	{
		_logger.error("HttpControl: out of memory", ILogger::ErrorSeverity::error);
		srv.send(HTTP_INTERNAL_SERVER_ERROR, "text/plain", "Out of memory");
		return;
	}
	
	srv.send(HTTP_OK, "text/json", _buf);
}

void HttpControl::outputs_state_set(WebServer& srv)
{
	assert(_lightController_p != nullptr);
	
	const auto& channelName = std::string(srv.arg("channel").c_str());
	if (channelName.empty())
	{
		_logger.error("HttpControl set output state failed: no channel specified", ILogger::ErrorSeverity::warning);
		srv.send(HTTP_BAD_REQUEST, "text/plain", "No channel specified");
		
		return;
	}
	
	if (!_lightController_p->isOutputChannelExists(channelName))
	{
		_logger.error("HttpControl set output state failed: invalid channel specified", ILogger::ErrorSeverity::warning);
		srv.send(HTTP_BAD_REQUEST, "text/plain", "Invalid channel specified");
		
		return;
	}
	
	auto sChannelState = srv.arg("state-on");
	sChannelState.toLowerCase();
	
	auto channelState = DiscreteState::unknown;
	
	if (sChannelState == "true")
	{
		channelState = DiscreteState::on;
	}
	else if (sChannelState == "false")
	{
		channelState = DiscreteState::off;
	}
	
	if (channelState == DiscreteState::unknown)
	{
		_logger.error("HttpControl set output state failed: state is empty or invalid", ILogger::ErrorSeverity::warning);
		srv.send(HTTP_BAD_REQUEST, "text/plain", "state-on is empty or invalid");
		
		return;
	}
	
	const auto ok = _lightController_p->setOutputState(std::string(channelName.c_str()), channelState);
	if (!ok)
	{
		_logger.error("HttpControl set output state failed", ILogger::ErrorSeverity::error);
		srv.send(HTTP_INTERNAL_SERVER_ERROR, "text/plain", "");
		
		return;
	}
	
	{
		LogInfo msg(_logger);
		
		msg.addText("HTTP API channel state set request. ");
		msg.addText("Channel: " + channelName);
		msg.addText(", state: ");
		msg.addText(channelState == DiscreteState::on ? "on" : "off");
	}
	
	srv.send(HTTP_OK, "text/json", "");
}
	
void HttpControl::outputs_local_off(WebServer& srv)
{
	assert(_lightController_p != nullptr);
	
	const auto ok = _lightController_p->localOff();
	if (!ok)
	{
		_logger.error("HttpControl local off failed", ILogger::ErrorSeverity::error);
		srv.send(HTTP_INTERNAL_SERVER_ERROR, "text/plain", "");
		
		return;
	}
	
	_logger.info("HTTP API all-off request. ");
	
	srv.send(HTTP_OK, "text/json", "");
}

HttpControl::HttpControl()
{
	_buf.reserve(1024);
}
