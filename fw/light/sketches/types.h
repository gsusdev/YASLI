#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdbool>

#include <string>
#include <vector>

#include <functional>

enum class EventType
{
    none,
    rise,
    fall 
};

enum class ActionType
{
    none,
    turnOn,
    turnOff,
    toggle,
    localOff,
    globalOff
};

enum class DiscreteState
{
    unknown,
    on,
    off
};

struct RuleCondition
{
    size_t inputChannelIndex;
    EventType eventType;
};

struct RuleAction
{
    size_t outputChannelIndex;
    ActionType actionType;
};

struct Rule
{
    RuleCondition condition;
    RuleAction action;
};

enum class ActionManagerError
{
    none,
    invalidChannelIndex,
    tooManyChannels,
    tooManyRules
};

enum class EventDetectorError
{
    none,
    invalidChannelIndex,
    tooManyChannels
};

class IInputEventListener
{
public:
	virtual ~IInputEventListener() = default;
	virtual void inputEventNotification(size_t channelIndex, EventType event) = 0;
};

class IEventDetector
{
public:
    virtual ~IEventDetector() = default;

    virtual EventDetectorError setInputState(size_t channelIndex, DiscreteState value) = 0;
    virtual EventDetectorError getOutputEvent(size_t channelIndex, EventType& result_out) = 0;

    virtual EventDetectorError execute() = 0;
};

class IEventDetectorConfigurator
{
public:
    virtual ~IEventDetectorConfigurator() = default;

    virtual EventDetectorError setChannelCount(size_t value) = 0;
};

class IActionManager
{
public:
    virtual ~IActionManager() = default;

    virtual ActionManagerError setInputEvent(size_t inputChannelIndex, EventType event) = 0;
    virtual ActionManagerError getOutputState(size_t outputChannelIndex, DiscreteState& outputState_out) = 0;
    virtual ActionManagerError forceOutput(size_t outputChannelIndex, DiscreteState state) = 0;
    virtual ActionManagerError forceLocalOff() = 0;
    virtual ActionManagerError forceGlobalOff() = 0;
    virtual ActionManagerError isLocalOff(bool& result_out) = 0;
    virtual ActionManagerError isGlobalOff(bool& result_out) = 0;

    virtual ActionManagerError execute() = 0;
};

class IActionManagerConfigurator
{
public:
    virtual ~IActionManagerConfigurator() = default;

    virtual ActionManagerError setInputChannelCount(size_t value) = 0;
    virtual ActionManagerError setOutputChannelCount(size_t value) = 0;

    virtual ActionManagerError addRule(const Rule& rule) = 0;
    virtual ActionManagerError clearRules() = 0;
};

class IInputFilter
{
public:
    virtual ~IInputFilter() = default;

    virtual void update(DiscreteState value, int timeElapsed_msec) = 0;
    virtual DiscreteState resultingState() const = 0;
};

class IRulesReader
{
public:
	enum class ReadResult
	{
		success,
		noData,
		error,
		skipped
	};
	
    virtual ~IRulesReader() = default;

	virtual ReadResult readRule(Rule& result_out) = 0;
    virtual bool hasMoreRules() const = 0;
    virtual bool reset() = 0;
};

class IChannelNameResolver
{
public:
    virtual ~IChannelNameResolver() = default;

    virtual bool resolveChannelName(const std::string& channelName, size_t& channelIndex_out) const = 0;
};

class IEventNameResolver
{
public:
    virtual ~IEventNameResolver() = default;

    virtual bool resolveEventName(const std::string& eventName, EventType& eventType_out) const = 0;
};

class IActionNameResolver
{
public:
    virtual ~IActionNameResolver() = default;

    virtual bool resolveActionName(const std::string& actionName, ActionType& actionType_out) const = 0;
};

class ILogger
{
public:
	enum class ErrorSeverity
	{
		warning,
		error,
		critical
	};
	
	virtual ~ILogger() = default;
	
	virtual void trace(const std::string& text, bool isFinal = true) = 0;
	virtual void error(const std::string& text, ErrorSeverity severity, bool isFinal = true) = 0;
	virtual void info(const std::string& text, bool isFinal = true) = 0;
	
	virtual void endMessage() = 0;
};

class ILogWriter
{
public:
	virtual ~ILogWriter() = default;
	virtual void write(const std::string& text) = 0;
};

class IEventSender
{
public:
    virtual ~IEventSender() = default;

    virtual void beginSendEvents() = 0;
    virtual void sendEvent(size_t inputChannelIndex, EventType eventType) = 0;
    virtual void endSendEvents() = 0;
};

class IGlobalOffSender
{
public:
    virtual ~IGlobalOffSender() = default;
    virtual void sendGlobalOff() = 0;
};

class IInputDevice
{
public:
    virtual ~IInputDevice() = default;

    virtual bool getCurrentValue(size_t channelIndex, DiscreteState& curValue_out) const = 0;
};

class IOutputDevice
{
public:
    virtual ~IOutputDevice() = default;

    virtual void setCurrentValue(size_t channelIndex, DiscreteState value) = 0;
};

enum class SettingDataType
{
	unknown,
	text,
	integral,
	real,
	boolean
};

class ISettingsGroup
{
public:
	virtual ~ISettingsGroup() = default;
		
	virtual size_t getItemCount() const = 0;
	virtual const std::string& getItemName(size_t index) const = 0;	
	virtual bool hasItem(const std::string& name) const = 0;
	
	virtual void reset() = 0;
	
	virtual bool getItemText(const std::string& name, std::string& value_out) const = 0;
	virtual bool setItemText(const std::string& name, const std::string& value, std::string* errorText_out_p = nullptr) = 0;
};

class ISettingValue
{
public:
	virtual ~ISettingValue() = default;
	
	virtual std::string toString() const = 0;
	virtual bool parse(const std::string& value, std::string* errorText_out_p = nullptr) = 0;
	
	virtual std::string getCaption() const = 0;
	virtual std::string getValueHint() const = 0;
	
	virtual bool isEmpty() const = 0;
	virtual void clear() = 0;
};

class ILightControllerFacade
{
public:
	virtual ~ILightControllerFacade() = default;
	
	virtual void registerInputEventListener(IInputEventListener& listener) = 0;
	virtual void unregisterInputEventListener(IInputEventListener& listener) = 0;
	
	virtual size_t getInputCount() const = 0;
	virtual size_t getOutputCount() const = 0;
	
	virtual void getInputNames(std::function<void (const std::string* name_p)> inserter) const = 0;	
	virtual void getOutputNames(std::function<void(const std::string* name_p)> inserter) const = 0;
	
	virtual bool isInputChannelExists(const std::string& name) const = 0;
	virtual bool isOutputChannelExists(const std::string& name) const = 0;
	
	virtual DiscreteState getInputState(const std::string& name) const = 0;
	
	virtual DiscreteState getOutputState(const std::string& name) const = 0;
	virtual bool setOutputState(const std::string& name, DiscreteState value) = 0;
	
	virtual bool localOff() = 0;
};