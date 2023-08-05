#include "log_utils.h"

#include "default_settings.h"
#include "light_controller.h"
#include "inputs.h"
#include "outputs.h"
#include "rules_reader.h"
#include "channels.h"
#include "channels_reader.h"
#include "standard_resolvers.h"

#include "main_stuff.h" 

static const String inputChannelParamsFilename = "/input_channels.txt";
static const String outputChannelParamsFilename = "/output_channels.txt";
static const String rulesParamsFilename = "/rules.txt";

static LightController lightController;
static InputDevice inputDevice;
static OutputDevice outputDevice; 
static RulesTextReader rulesReader; 

static InputChannelList inputChannelParams;
static OutputChannelList outputChannelParams;

static ChannelsReader<InputChannelInfo> inputChannelParamsReader;
static ChannelsReader<OutputChannelInfo> outputChannelParamsReader;

static StandardResolvers standardResolvers;

static DefaultSettingsCreator defSettingsCreator;

void LightControllerFacade::registerInputEventListener(IInputEventListener& listener)
{
	lightController.registerInputEventListener(listener);
}

void LightControllerFacade::unregisterInputEventListener(IInputEventListener& listener)
{
	lightController.unregisterInputEventListener(listener);	
}		

size_t LightControllerFacade::getInputCount() const
{
	const auto result = lightController.getInputCount();
	return result;
}

size_t LightControllerFacade::getOutputCount() const
{
	const auto result = lightController.getOutputCount();
	return result;
}

void LightControllerFacade::getInputNames(std::function<void(const std::string* name_p)> inserter) const
{
	const auto channelCount = inputDevice.getChannelCount();
	for (size_t i = 0; i < channelCount; ++i)
	{
		std::string name;
		if (!inputChannelParams.getChannelName(i, name))
		{
			name = std::string();
		}
		
		inserter(&name);
	}
}

void LightControllerFacade::getOutputNames(std::function<void(const std::string* name_p)> inserter) const
{
	const auto channelCount = outputDevice.getChannelCount();
	for (size_t i = 0; i < channelCount; ++i)
	{
		std::string name;
		if (!outputChannelParams.getChannelName(i, name))
		{
			name = std::string();
		}
		
		inserter(&name);
	}
}
	
bool LightControllerFacade::isInputChannelExists(const std::string& name) const
{
	size_t indx;
	const auto result = inputChannelParams.resolveChannelName(name, indx);
	return result;
}

bool LightControllerFacade::isOutputChannelExists(const std::string& name) const
{
	size_t indx;
	const auto result = outputChannelParams.resolveChannelName(name, indx);
	return result;
}

DiscreteState LightControllerFacade::getInputState(const std::string& name) const
{
	size_t indx;
	const auto ok = inputChannelParams.resolveChannelName(name, indx);
	if (!ok)
	{
		return DiscreteState::unknown;
	}
	
	const auto result = lightController.getInputState(indx);
	return result;
}
	
DiscreteState LightControllerFacade::getOutputState(const std::string& name) const
{
	size_t indx;
	const auto ok = outputChannelParams.resolveChannelName(name, indx);
	if (!ok)
	{
		return DiscreteState::unknown;
	}
	
	const auto result = lightController.getOutputState(indx);
	return result;
}

bool LightControllerFacade::setOutputState(const std::string& name, DiscreteState value)
{
	size_t indx;
	auto result = outputChannelParams.resolveChannelName(name, indx);
	if (!result)
	{
		return false;
	}
		
	lightController.forceOutput(indx, value);
	return true;
}
	
bool LightControllerFacade::localOff()
{	
	lightController.forceLocalOff();
	return true;
}
	
bool LightControllerFacade::initialize(ILogger* logger_p, FS* fileSystem_p, bool initializeDefaults)
{
	LoggerHelper logger;
	logger.setLogger(logger_p);
	
	if (initializeDefaults)
	{
		logger.trace("Initializing light defaults");
	
		defSettingsCreator.setLogger(&logger);
		defSettingsCreator.setFileSystem(fileSystem_p);
		defSettingsCreator.setInputsFilename(inputChannelParamsFilename);
		defSettingsCreator.setOutputsFilename(outputChannelParamsFilename);
		defSettingsCreator.setRulesFilename(rulesParamsFilename);
		defSettingsCreator.setOverwriteIfExists(true);
		if (!defSettingsCreator.execute())
		{
			logger.error("Default light settings creation failed", ILogger::ErrorSeverity::critical);
			return false;
		}
	}
	
	auto ok = true;
	
	logger.trace("Reading input channels parameters");	
	
	inputChannelParamsReader.setChannelList(&inputChannelParams);
	inputChannelParamsReader.setFileSystem(fileSystem_p);
	inputChannelParamsReader.setFilename(inputChannelParamsFilename);
	if (!inputChannelParamsReader.execute())
	{
		logger.error("Reading input channels parameters failed", ILogger::ErrorSeverity::error);
		ok = false;
	}
		
	logger.trace("Reading output channels parameters");	
	
	outputChannelParamsReader.setChannelList(&outputChannelParams);
	outputChannelParamsReader.setFileSystem(fileSystem_p);
	outputChannelParamsReader.setFilename(outputChannelParamsFilename);
	if (!outputChannelParamsReader.execute())
	{
		logger.error("Reading output channels parameters failed", ILogger::ErrorSeverity::error);
		ok = false;
	}
			
	rulesReader.setLogger(logger_p);
	rulesReader.setInputChannelResolver(&inputChannelParams);
	rulesReader.setOutputChannelResolver(&outputChannelParams);
	rulesReader.setEventResolver(&standardResolvers);
	rulesReader.setActionResolver(&standardResolvers);
	rulesReader.setFilename(rulesParamsFilename);
	rulesReader.setFileSystem(fileSystem_p);
	
	logger.trace("Initializing input channels");	
	inputDevice.setLogger(logger_p);
	if (!inputDevice.initialize())
	{
		logger.error("Input device initialization failed", ILogger::ErrorSeverity::error);
		ok = false;
	}
	
	logger.trace("Initializing output channels");	
	outputDevice.setLogger(logger_p);
	if (!outputDevice.initialize())
	{
		logger.error("Output device initialization failed", ILogger::ErrorSeverity::error);
		ok = false;
	}
	
	const auto inputCount = inputDevice.getChannelCount();
	const auto outputCount = outputDevice.getChannelCount();
		
	lightController.setLogger(logger_p);
	
	lightController.setInputCount(inputCount);
	lightController.setOutputCount(outputCount);
	
	lightController.setInputDevice(&inputDevice);
	lightController.setOutputDevice(&outputDevice);
	
	lightController.setRulesReader(&rulesReader);
			
	logger.trace("Initializing rules manager");
	if (!lightController.initialize())
	{
		logger.error("Rules manager initialization failed", ILogger::ErrorSeverity::error);
		ok = false;
	}	
	
	return ok;
}

void LightControllerFacade::loop(int timeElapsed_msec)
{
	inputDevice.update();
	lightController.execute(timeElapsed_msec);
	outputDevice.update();
}