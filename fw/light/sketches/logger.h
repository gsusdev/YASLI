#pragma once

#include <cstdint>
#include <cstddef>
#include <cstdbool>
#include <cassert>

#include <string>

#include "types.h"
 
class Logger : public ILogger
{
public: 
	Logger();
	void setWriter(ILogWriter* value_p) { _logWriter_p = value_p; }
	
	void setIncludeTrace(bool value) { _includeTrace = value; }	
	void setIncludeErrors(bool value) { _includeErrors = value; }
	void setIncludeInfo(bool value) { _includeInfo = value; }
	
	void trace(const std::string& text, bool isFinal = true) override;
	void error(const std::string& text, ErrorSeverity severity, bool isFinal = true) override;
	void info(const std::string& text, bool isFinal = true) override;
	
	void endMessage() override;
	
private:
	void write(const std::string& text, bool isFinal);
	void beginMessage();
	
	ILogWriter* _logWriter_p = nullptr;
	
	bool _includeTrace = true;
	bool _includeErrors = true;
	bool _includeInfo = true;
	
	bool _isMessage = false;
};