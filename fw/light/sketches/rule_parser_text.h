#pragma once

#include <cstdint>
#include <cstddef>
#include <cstdbool>

#include <string>
#include <set>

#include "types.h"
#include "log_utils.h"

enum class RuleParserError
{
    none,
    internalError,
    parsingError,
    invalidInputChannel,
    invalidOutputChannel,
    invalidEvent,
    invalidAction
};

class RuleTextParser
{
public:
    using Errors = std::set<RuleParserError>;

    RuleTextParser();
		
    void setInputChannelResolver(IChannelNameResolver* value_p) { _inputChannelResolver_p = value_p; }
    void setOutputChannelResolver(IChannelNameResolver* value_p) { _outputChannelResolver_p = value_p; }
    void setEventResolver(IEventNameResolver* value_p) { _eventResolver_p = value_p; }
    void setActionResolver(IActionNameResolver* value_p) { _actionResolver_p = value_p; }
 
    const Errors& errors() const { return _errors; }
    bool parse(const std::string& text, Rule& result_out);

private:
    bool readInput(const std::string& text, Rule& rule);
    bool readEvent(const std::string& text, Rule& rule);
    bool readOutput(const std::string& text, Rule& rule);
    bool readAction(const std::string& text, Rule& rule);

    IChannelNameResolver* _inputChannelResolver_p = nullptr;
    IChannelNameResolver* _outputChannelResolver_p = nullptr;

    IEventNameResolver* _eventResolver_p = nullptr;
    IActionNameResolver* _actionResolver_p = nullptr;
	
    Errors _errors;
};
