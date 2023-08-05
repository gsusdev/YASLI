#include <cassert>

#include "log_utils.h"

// **************************************** LoggerHelper ****************************************

LoggerHelper::LoggerHelper()
	: ILogger()
{}
	
void LoggerHelper::trace(const std::string& text, bool isFinal) 
{ 
	if (_logger_p != nullptr) 
	{ 
		_logger_p->trace(text, isFinal); 
	} 
} 
	
void LoggerHelper::error(const std::string& text, ILogger::ErrorSeverity severity, bool isFinal) 
{
	if (_logger_p != nullptr) 
	{
		_logger_p->error(text, severity, isFinal); 
	}
}
	
void LoggerHelper::info(const std::string& text, bool isFinal) 
{ 
	if (_logger_p != nullptr) 
	{
		_logger_p->info(text, isFinal); 
	}
}

void LoggerHelper::endMessage()
{
	if (_logger_p != nullptr) 
	{
		_logger_p->endMessage(); 
	}
}




