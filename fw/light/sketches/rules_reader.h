#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdbool>

#include <Arduino.h>
#include <FS.h>

#include "types.h"
#include "rule_parser_text.h"

class RulesTextReader : public IRulesReader
{ 
public:
	RulesTextReader();
	
	void setLogger(ILogger* value_p) { _logger.setLogger(value_p); }
	
	void setInputChannelResolver(IChannelNameResolver* value_p);
	void setOutputChannelResolver(IChannelNameResolver* value_p);
	void setEventResolver(IEventNameResolver* value_p);
	void setActionResolver(IActionNameResolver* value_p);
		
	void setFilename(String value) { _filename = value; }
	void setFileSystem(FS* value_p) { _fileSystem_p = value_p; }
	
	ReadResult readRule(Rule& result_out) override;
	bool hasMoreRules() const override { return _hasRule; }
	bool reset() override;
	
private:
	void writeParserErrorsToLog(const std::string& ruleText);
	
	LoggerHelper _logger;
	
	String _filename;
	FS* _fileSystem_p = nullptr;
	File _curFile;
	
	RuleTextParser _ruleParser;
	
	bool _hasRule = false;
};