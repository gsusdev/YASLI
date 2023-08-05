#include <cassert>
#include <map>

#include "string_utils.h"

#include "log_utils.h"
#include "rules_reader.h"

RulesTextReader::RulesTextReader() :
	IRulesReader()
{
}

void RulesTextReader::setInputChannelResolver(IChannelNameResolver* value_p)
{
	_ruleParser.setInputChannelResolver(value_p);
}

void RulesTextReader::setOutputChannelResolver(IChannelNameResolver* value_p)
{
	_ruleParser.setOutputChannelResolver(value_p);
}

void RulesTextReader::setEventResolver(IEventNameResolver* value_p)
{
	_ruleParser.setEventResolver(value_p);
} 

void RulesTextReader::setActionResolver(IActionNameResolver* value_p)
{
	_ruleParser.setActionResolver(value_p);
}


IRulesReader::ReadResult RulesTextReader::readRule(Rule& result_out)
{
	auto ok = true;
	ok &= _curFile.available() > 0;
	
	if (!ok)
	{
		_hasRule = false;
		return ReadResult::noData;
	}
	
	auto line = _curFile.readStringUntil('\n');
	_hasRule = _curFile.available() > 0;
	if (!_hasRule)
	{
		_curFile.close();
	}
		
	std::string s(line.c_str());
	StringUtils::trim(s);
	
	if (s.empty())
	{
		return ReadResult::skipped;		
	}
	
	if (s.size() < 2)
	{
		LogError msg(_logger, ILogger::ErrorSeverity::warning);
			
		msg.addText("Invalid rule string: ");
		msg.addText(s);
		
		return ReadResult::error;
	}
	
	if (s.substr(0, 2) == "//")
	{
		return ReadResult::skipped;
	}
	
	ok &= _ruleParser.parse(s, result_out);
	if (!ok)
	{
		_logger.error("Invalid rule string: ", ILogger::ErrorSeverity::warning, false);
		_logger.error(s, ILogger::ErrorSeverity::warning, false);
		_logger.error("\n( ", ILogger::ErrorSeverity::warning, false);
		
		writeParserErrorsToLog(s);			
		
		_logger.error(" )", ILogger::ErrorSeverity::warning, true);
		
		return ReadResult::error;
	}
	
	return ReadResult::success;
}


bool RulesTextReader::reset()
{
	assert(_fileSystem_p != nullptr);
	
	_hasRule = false;
	_curFile.close();	
	
	_curFile = _fileSystem_p->open(_filename);
	if (!_curFile)
	{
		_logger.error("Failed to open rules file" + std::string(_filename.c_str()), ILogger::ErrorSeverity::error);		
		_hasRule = false;
		
		return false;
	}
	
	_hasRule = _curFile.available() > 0;
	return true;
}


void RulesTextReader::writeParserErrorsToLog(const std::string& ruleText)
{
	static const std::map<RuleParserError, std::string> errorTexts = 
	{
		{ RuleParserError::internalError,        "internal error"},
		{ RuleParserError::invalidAction,        "invalid action"},
		{ RuleParserError::invalidEvent,         "invalid event"},
		{ RuleParserError::invalidInputChannel,  "invalid input"},
		{ RuleParserError::invalidOutputChannel, "invalid output"},
		{ RuleParserError::parsingError,         "syntax error"},
	};
	
	LogError err(_logger, ILogger::ErrorSeverity::error, false);
	
	auto first = true;
	for (const auto error : _ruleParser.errors())
	{
		if (!first)
		{
			err.addText(", ");
			first = false;
		}
		
		const auto& text = errorTexts.at(error);
		err.addText(text);
	}	
}
