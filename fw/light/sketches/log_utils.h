#pragma once

#include <cstdint>
#include <cstddef>
#include <cstdbool>
#include <cassert>

#include <string>

#include "types.h"

class LoggerHelper : public ILogger
{
public:
	LoggerHelper(); 
	
	void setLogger(ILogger* value_p) { _logger_p = value_p; }
	
	void trace(const std::string& text, bool isFinal = true) override;
	void error(const std::string& text, ILogger::ErrorSeverity severity, bool isFinal = true) override;
	void info(const std::string& text, bool isFinal = true) override;
	
	void endMessage() override;
	
private:
	ILogger* _logger_p = nullptr;
};


enum class LogMessageKind
{
	trace,
	error,
	info
};

template <LogMessageKind kind>
class LogMessage
{
public:
	template<LogMessageKind K = kind>
	LogMessage(ILogger& logger, ILogger::ErrorSeverity errorSeverity, bool autoFinish = true,
		typename std::enable_if<K == LogMessageKind::error>::type* = 0)			
		: _logger(logger)
		, _errorSeverity(errorSeverity)
		, _autoFinish(autoFinish)
	{}		
	
	LogMessage(ILogger& logger, bool autoFinish = true)	: 
		_logger(logger),
		_autoFinish(autoFinish)
	{}		
	
	~LogMessage()
	{
		if (_autoFinish)
		{
			_logger.endMessage();
		}
	}
	
	void addText(const std::string& text)
	{
		switch (kind)
		{
			case LogMessageKind::error:
				_logger.error(text, _errorSeverity, false);
				break;
			
			case LogMessageKind::info:
				_logger.info(text, false);
				break;
			
			case LogMessageKind::trace:
				_logger.trace(text, false);
				break;
			
			default:
				assert(false);
		}
	}
	
private:
	ILogger& _logger;
	ILogger::ErrorSeverity _errorSeverity = ILogger::ErrorSeverity::error;
	bool _autoFinish;
};

using LogError = LogMessage<LogMessageKind::error>;
using LogInfo = LogMessage<LogMessageKind::info>;
using LogTrace = LogMessage<LogMessageKind::trace>;