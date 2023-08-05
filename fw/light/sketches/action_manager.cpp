#include "action_manager.h"

ActionManager::ActionManager() :
    IActionManager(),
    IActionManagerConfigurator()
{
    _inLocalOff = false; 
    _inGlobalOff = false;

    _outLocalOff = false;
    _outGlobalOff = false;
}

ActionManagerError ActionManager::setInputEvent(size_t inputChannelIndex, EventType event)
{
    if (inputChannelIndex >= _curInputEvents.size())
    {
        return ActionManagerError::invalidChannelIndex;
    }

    _curInputEvents[inputChannelIndex] = event;

    return ActionManagerError::none;
}

ActionManagerError ActionManager::getOutputState(size_t outputChannelIndex, DiscreteState& outputState_out)
{
    if (outputChannelIndex >= _curOutputStates.size())
    {
        return ActionManagerError::invalidChannelIndex;
    }

    outputState_out = _curOutputStates[outputChannelIndex];

    return ActionManagerError::none;
}

ActionManagerError ActionManager::forceOutput(size_t outputChannelIndex, DiscreteState state)
{
    if (outputChannelIndex >= _curForcedOutputStates.size())
    {
        return ActionManagerError::invalidChannelIndex;
    }

    _curForcedOutputStates[outputChannelIndex] = state;

    return ActionManagerError::none;
}

ActionManagerError ActionManager::forceLocalOff()
{
    _inLocalOff = true;
    return ActionManagerError::none;
}

ActionManagerError ActionManager::forceGlobalOff()
{
    _inGlobalOff = true;
    return ActionManagerError::none;
}

ActionManagerError ActionManager::isLocalOff(bool& result_out)
{
    result_out = _outLocalOff;
    return ActionManagerError::none;
}

ActionManagerError ActionManager::isGlobalOff(bool& result_out)
{
    result_out = _outGlobalOff;
    return ActionManagerError::none;
}

ActionManagerError ActionManager::addRule(const Rule& rule)
{
    if (_rules.size() >= _maxRuleCount)
    {
        return ActionManagerError::tooManyRules;
    }

    _rules.push_back(rule);

    return ActionManagerError::none;
}

ActionManagerError ActionManager::clearRules()
{
    _rules.clear();
    return ActionManagerError::none;
}

ActionManagerError ActionManager::setInputChannelCount(size_t value)
{
    if (value > _maxInputChannelCount)
    {
        return ActionManagerError::tooManyChannels;
    }

    _curInputEvents.resize(value);
    resetInputEvents();

    return ActionManagerError::none;
}

ActionManagerError ActionManager::setOutputChannelCount(size_t value)
{
    if (value > _maxOutputChannelCount)
    {
        return ActionManagerError::tooManyChannels;
    }

    _curOutputStates.resize(value);
    _curForcedOutputStates.resize(value);

    resetInputForcedStates();

    return ActionManagerError::none;
}

ActionManagerError ActionManager::execute()
{
    _outLocalOff = false;
    _outGlobalOff = false;

    applyRules();
    applyForcedStates();

    if (_inLocalOff)
    {
        executeLocalOff();
    }

    if (_inGlobalOff)
    {
        executeGlobalOff();
    }

    resetInputForcedStates();
    resetInputEvents();

    return ActionManagerError::none;
}

void ActionManager::applyRules()
{
    for (const auto& rule : _rules)
    {
        if (isRuleConditionMatches(rule.condition))
        {
            if (!executeRuleAction(rule.action))
            {
                //TODO: report error
            }
        }
    }
}

void ActionManager::applyForcedStates()
{
    for (size_t i = 0; i < _curForcedOutputStates.size(); ++i)
    {
        const auto state = _curForcedOutputStates[i];
        if (state == DiscreteState::unknown)
        {
            continue;
        }

        _curOutputStates[i] = state;
    }
}

bool ActionManager::isRuleConditionMatches(const RuleCondition& condition) const
{
    if (condition.eventType == EventType::none)
    {
        return false;
    }

    const auto channelIndex = condition.inputChannelIndex;
    if (channelIndex >= _curInputEvents.size())
    {
        return false;
    }

    auto match = condition.eventType == _curInputEvents[channelIndex];

    return match;
}

bool ActionManager::executeRuleAction(const RuleAction& action)
{
    if (action.actionType == ActionType::none)
    {
        return true;
    }

    if (action.actionType == ActionType::localOff)
    {
        executeLocalOff();
    }
    else if (action.actionType == ActionType::globalOff)
    {
        executeGlobalOff();
    }
    else
    {
        const auto channelIndex = action.outputChannelIndex;
        if (channelIndex >= _curOutputStates.size())
        {
            return false;
        }

        auto state = _curOutputStates[channelIndex];

        if (action.actionType == ActionType::turnOn)
        {
            state = DiscreteState::on;
        }
        else if (action.actionType == ActionType::turnOff)
        {
            state = DiscreteState::off;
        }
        else if (action.actionType == ActionType::toggle)
        {
            if (state == DiscreteState::on)
            {
                state = DiscreteState::off;
            }
            else if (state == DiscreteState::off)
            {
                state = DiscreteState::on;
            }
            else
            {
                state = DiscreteState::off;
            }
        }

        _curOutputStates[channelIndex] = state;
    }

    return true;
}

void ActionManager::resetInputForcedStates()
{
    _inLocalOff = false;
    _inGlobalOff = false;

    for (auto it = _curForcedOutputStates.begin(); it != _curForcedOutputStates.end(); ++it)
    {
        *it = DiscreteState::unknown;
    }
}

void ActionManager::resetInputEvents()
{
    for (auto it = _curInputEvents.begin(); it != _curInputEvents.end(); ++it)
    {
        *it = EventType::none;
    }
}

void ActionManager::setAllOff()
{
    for (auto it = _curOutputStates.begin(); it != _curOutputStates.end(); ++it)
    {
        *it = DiscreteState::off;
    }
}

void ActionManager::executeLocalOff()
{
    setAllOff();
    _outLocalOff = true;
}

void ActionManager::executeGlobalOff()
{
    setAllOff();
    _outGlobalOff = true;
}
