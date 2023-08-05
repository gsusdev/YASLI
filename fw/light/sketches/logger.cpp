#include <cassert>
#include <map>

#include "logger.h"

Logger::Logger() : 
	ILogger()
{
}

void Logger::trace(const std::string& text, bool isFinal)
{
	if (!_includeTrace)
	{
		return;
	}
	
	write(text, isFinal);	
}

void Logger::error(const std::string& text, ErrorSeverity severity, bool isFinal)
{
	if (!_includeErrors)
	{ 
		return;
	}
	
	static const std::map<ILogger::ErrorSeverity, std::string> errorHeaders = 
	{
		{ ILogger::ErrorSeverity::critical, "[CRITICAL]\t" },
		{ ILogger::ErrorSeverity::error,    "[ERROR]\t"    },
		{ ILogger::ErrorSeverity::warning,  "[WARNING]\t"  },
	};
	
	write(errorHeaders.at(severity) + text, isFinal);
}

void Logger::info(const std::string& text, bool isFinal)
{
	if (!_includeInfo)
	{
		return;
	}
	
	write("[INFO] " + text, isFinal);
}
	
void Logger::beginMessage()
{
	_isMessage = true;
}

void Logger::endMessage()
{
	_isMessage = false;
	_logWriter_p->write("\n");
}

void Logger::write(const std::string& text, bool isFinal)
{
	assert(_logWriter_p != nullptr);
	
	if (!_isMessage && !isFinal)
	{
		beginMessage();
	}
	
	_logWriter_p->write(text);
	
	if (!_isMessage)
	{
		_logWriter_p->write("\n");
	}
	
	if (_isMessage && isFinal)
	{
		endMessage();
	}
}