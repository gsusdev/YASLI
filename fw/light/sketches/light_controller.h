#pragma once

#include <cstdint>
#include <cstddef>
#include <cstdbool>

#include <vector>
#include <list>
#include <memory>

#include "types.h" 

#include "action_manager.h"
#include "event_detector.h"
#include "input_filter_ac.h"

#include "log_utils.h"

class LightController
{
public:
	void setLogger(ILogger* value_p) { _logger.setLogger(value_p); }
	
	void registerInputEventListener(IInputEventListener& listener) { _inputEventListeners.push_back(&listener); }
	void unregisterInputEventListener(IInputEventListener& listener) { _inputEventListeners.remove(&listener); }
	
	size_t getInputCount() const { return _inputChannelCount; }
	void setInputCount(size_t value) { _inputChannelCount = value; }
    
	size_t getOutputCount() const { return _outputChannelCount; }
	void setOutputCount(size_t value) { _outputChannelCount = value; }

    void setInputDevice(IInputDevice* value_p) { _inputDevice_p = value_p; }
    void setOutputDevice(IOutputDevice* value_p) { _outputDevice_p = value_p; }

    void setRulesReader(IRulesReader* value_p) { _rulesReader_p = value_p; }
    void setGlobalOffSender(IGlobalOffSender* value_p) { _globalOffSender_p = value_p; }
    void setEventSender(IEventSender* value_p) { _eventSender_p = value_p; }

    bool initialize();
    void execute(int time_elapsed_ms);

    void forceOutput(size_t outputChannelIndex, DiscreteState state);
    void forceLocalOff();
	
	DiscreteState getInputState(size_t index) const;	
	DiscreteState getOutputState(size_t index) const;

private:
    using PInputFilter = std::unique_ptr<InputFilterAc>;
	
	void logEvent(int channelIndex, EventType event);
	void logOutputChange(int channelIndex, DiscreteState state);

    bool initializeFilters();
    bool readRules();

    void readInputs(int time_elapsed_ms);
    void detectEvents(int time_elapsed_ms);
    void manageActions(int time_elapsed_ms);
    void writeOutputs();
    void sendEvents();
    void sendGlobalOff();
	
	LoggerHelper _logger;

    IInputDevice* _inputDevice_p = nullptr;
    IOutputDevice* _outputDevice_p = nullptr;

    IRulesReader* _rulesReader_p = nullptr;
    IGlobalOffSender* _globalOffSender_p = nullptr;
    IEventSender* _eventSender_p = nullptr;

    ActionManager _actionManager;
    EventDetector _eventDetector;

    size_t _inputChannelCount = 0;
    size_t _outputChannelCount = 0;

	
    std::vector<PInputFilter> _inputFilters;
	std::vector<DiscreteState> _lastInputStates;
	std::vector<DiscreteState> _lastOutputStates;
	std::list<IInputEventListener*> _inputEventListeners;
};
