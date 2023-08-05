#include <cassert>
#include <map>

#include "string_utils.h"
#include "light_controller.h"

bool LightController::initialize()
{
	auto ok = true;
		
    ok &= initializeFilters();
	
	_lastInputStates.resize(_inputChannelCount);
	std::fill(_lastInputStates.begin(), _lastInputStates.end(), DiscreteState::unknown);
	
	_lastOutputStates.resize(_outputChannelCount);
	std::fill(_lastOutputStates.begin(), _lastOutputStates.end(), DiscreteState::unknown);
	
    _eventDetector.setChannelCount(_inputChannelCount);
    _actionManager.setInputChannelCount(_inputChannelCount);
    _actionManager.setOutputChannelCount(_outputChannelCount);

    ok &= readRules();
	
	return ok;
}
 
bool LightController::initializeFilters()
{
    if (_inputFilters.size() >= _inputChannelCount)
    {
        _inputFilters.resize(_inputChannelCount);
    }
    else
    {
        size_t needToAdd = _inputChannelCount - _inputFilters.size();
        _inputFilters.reserve(_inputChannelCount);

        for (size_t i = 0; i < needToAdd; ++i)
        {
            _inputFilters.push_back((PInputFilter)(new InputFilterAc()));
        }
    }
	
	return true;
}

bool LightController::readRules()
{
	assert(_rulesReader_p != nullptr);
        
    _actionManager.clearRules();
    _rulesReader_p->reset();

	auto ruleAdded = false;
    while (_rulesReader_p->hasMoreRules())
    {
        Rule newRule;
        const auto ok = _rulesReader_p->readRule(newRule) == IRulesReader::ReadResult::success;
        if (!ok)
        {
            continue;
        }

	    if (_actionManager.addRule(newRule) == ActionManagerError::none)
	    {
		    ruleAdded = true;
	    }
    }
	
	return ruleAdded;
}

void LightController::execute(int time_elapsed_ms)
{
    readInputs(time_elapsed_ms);

    detectEvents(time_elapsed_ms);
    manageActions(time_elapsed_ms);

    writeOutputs();

    sendEvents();
    sendGlobalOff();
}

void LightController::readInputs(int time_elapsed_ms)
{
    for (size_t i = 0; i < _inputChannelCount; ++i)
    {
        auto& filter_p = _inputFilters[i];

        DiscreteState state = DiscreteState::unknown;

        if (_inputDevice_p != nullptr)
        {
            const auto ok = _inputDevice_p->getCurrentValue(i, state);
            if (!ok)
            {
                state = DiscreteState::unknown;
            }
        }
	    
        filter_p->update(state, time_elapsed_ms);
    }
}

void LightController::detectEvents(int time_elapsed_ms)
{
    (void)time_elapsed_ms;

    for (size_t i = 0; i < _inputChannelCount; ++i)
    {
        DiscreteState state;

        state = _inputFilters[i]->resultingState();
        //_inputDevice_p->getCurrentValue(i, state);

	    _lastInputStates[i] = state;
        _eventDetector.setInputState(i, state);
    }

    _eventDetector.execute();
}

void LightController::manageActions(int time_elapsed_ms)
{
    (void)time_elapsed_ms;

    for (size_t i = 0; i < _inputChannelCount; ++i)
    {
        EventType event;
        const auto ok = _eventDetector.getOutputEvent(i, event) == EventDetectorError::none;
        if (!ok)
        {
            event = EventType::none;
        }
	    
	    if (event != EventType::none)
	    {
		    logEvent(i, event);
		    
		    for (auto& listener_p : _inputEventListeners)
		    {
			    listener_p->inputEventNotification(i, event);
		    }
	    }
	    
        _actionManager.setInputEvent(i, event);
    }

    _actionManager.execute();
}

void LightController::logEvent(int channelIndex, EventType event)
{
	static const std::map<EventType, std::string> eventTexts = 
	{
		{ EventType::none, "none"},
		{ EventType::fall, "fall"},
		{ EventType::rise, "rise"}
	};
	
	_logger.info("Input " + StringUtils::toString(channelIndex) + ", " + eventTexts.at(event) + " detected");
}

void LightController::writeOutputs()
{
    if (_outputDevice_p == nullptr)
    {
        return;
    }

    for (size_t i = 0; i < _outputChannelCount; ++i)
    {
        DiscreteState state;

        const auto ok = _actionManager.getOutputState(i, state) == ActionManagerError::none;
        if (!ok)
        {
            continue;
        }
	    	    
        _outputDevice_p->setCurrentValue(i, state);
	    if (state != _lastOutputStates[i])
	    {
		    logOutputChange(i, state);
		    _lastOutputStates[i] = state;
	    }	    
    }
}

void LightController::logOutputChange(int channelIndex, DiscreteState state)
{
	static const std::map<DiscreteState, std::string> stateTexts = 
	{
		{ DiscreteState::unknown, "UNKNOWN" },
		{ DiscreteState::on,      "ON"      },
		{ DiscreteState::off,     "OFF"     }
	};
	
	_logger.info("Output " + StringUtils::toString(channelIndex) + " switched to " + stateTexts.at(state));
}

void LightController::sendEvents()
{
    if (_eventSender_p == nullptr)
    {
        return;
    }

    _eventSender_p->beginSendEvents();

    for (size_t i = 0; i < _inputChannelCount; ++i)
    {
        EventType event;
        const auto ok = _eventDetector.getOutputEvent(i, event) == EventDetectorError::none;
        if (!ok || (event == EventType::none))
        {
            continue;
        }

        _eventSender_p->sendEvent(i, event);
    }

    _eventSender_p->endSendEvents();
}

void LightController::sendGlobalOff()
{
    if (_globalOffSender_p == nullptr)
    {
        return;
    }

    bool isGlobalOff;
    if (_actionManager.isGlobalOff(isGlobalOff) != ActionManagerError::none)
    {
        return;
    }

    if (!isGlobalOff)
    {
        return;
    }

    _globalOffSender_p->sendGlobalOff();
}

void LightController::forceOutput(size_t outputChannelIndex, DiscreteState state)
{
    _actionManager.forceOutput(outputChannelIndex, state);
}

void LightController::forceLocalOff()
{
    _actionManager.forceLocalOff();
}

DiscreteState LightController::getInputState(size_t index) const
{
	if (index > _lastInputStates.size())
	{
		return DiscreteState::unknown;
	}
	
	return _lastInputStates[index];
}

DiscreteState LightController::getOutputState(size_t index) const
{
	if (index > _lastOutputStates.size())
	{
		return DiscreteState::unknown;
	}
	
	return _lastOutputStates[index];
}
