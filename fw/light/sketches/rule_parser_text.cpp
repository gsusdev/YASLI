#include "string_utils.h"

#include "rule_parser_text.h"

enum class ParsingState
{
    waitInput,
    waitEvent,
    waitOutput,
    waitAction,
    finished
};

RuleTextParser::RuleTextParser()
{
}

bool RuleTextParser::parse(const std::string& text, Rule& result_out)
{
	assert(_inputChannelResolver_p != nullptr);
	assert(_outputChannelResolver_p != nullptr);
	assert(_eventResolver_p != nullptr);
	assert(_actionResolver_p != nullptr);
	 
	auto ok = true;

	_errors.clear();
	
    if (!ok)
    {
        _errors.insert(RuleParserError::internalError);
        return false;
    }

    result_out.action.actionType = ActionType::none;
    result_out.action.outputChannelIndex = 0;

    result_out.condition.eventType = EventType::none;
    result_out.condition.inputChannelIndex = 0;

    size_t pos = 0;
    ParsingState state = ParsingState::waitInput;
    std::string curWord;

    auto terminate = false;
    while (StringUtils::nextWord(text, pos, curWord))
    {
        if (terminate || (state == ParsingState::finished))
        {
            break;
        }

        switch (state)
        {
            case ParsingState::waitInput:
                readInput(curWord, result_out);
                state = ParsingState::waitEvent;
                break;

            case ParsingState::waitEvent:
                readEvent(curWord, result_out);
                state = ParsingState::waitOutput;
                break;

            case ParsingState::waitOutput:
                readOutput(curWord, result_out);
                state = ParsingState::waitAction;
                break;

            case ParsingState::waitAction:
                readAction(curWord, result_out);
                state = ParsingState::finished;
                break;

            default:
                _errors.insert(RuleParserError::internalError);
                terminate = true;
                break;
        }
    }

    if (
            (result_out.action.actionType == ActionType::localOff)
            ||
            (result_out.action.actionType == ActionType::globalOff)
       )
    {
        _errors.erase(RuleParserError::invalidOutputChannel);
    }

    if (state != ParsingState::finished)
    {
        _errors.insert(RuleParserError::parsingError);
    }

    return _errors.empty();
}

bool RuleTextParser::readInput(const std::string& text, Rule& rule)
{
    size_t channelIndex;
    const auto ok = _inputChannelResolver_p->resolveChannelName(text, channelIndex);

    if (!ok)
    {
        _errors.insert(RuleParserError::invalidInputChannel);
        return false;
    }

    rule.condition.inputChannelIndex = channelIndex;

    return true;
}

bool RuleTextParser::readEvent(const std::string& text, Rule& rule)
{
    EventType event;
    const auto ok = _eventResolver_p->resolveEventName(text, event);

    if (!ok)
    {
        _errors.insert(RuleParserError::invalidEvent);
        return false;
    }

    rule.condition.eventType = event;

    return true;
}

bool RuleTextParser::readOutput(const std::string& text, Rule& rule)
{
    size_t channelIndex;
    const auto ok = _outputChannelResolver_p->resolveChannelName(text, channelIndex);

    if (!ok)
    {
        _errors.insert(RuleParserError::invalidOutputChannel);
        return false;
    }

    rule.action.outputChannelIndex = channelIndex;

    return true;
}

bool RuleTextParser::readAction(const std::string& text, Rule& rule)
{
    ActionType action;
    const auto ok = _actionResolver_p->resolveActionName(text, action);

    if (!ok)
    {
        _errors.insert(RuleParserError::invalidEvent);
        return false;
    }

    rule.action.actionType = action;

    return true;
}
