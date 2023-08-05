#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdbool>

#include <string>

#include <FS.h>

#include "types.h"

class LightControllerFacade : public ILightControllerFacade
{
public:
	void registerInputEventListener(IInputEventListener& listener) override;
	void unregisterInputEventListener(IInputEventListener& listener) override;
	
	size_t getInputCount() const override;
	virtual size_t getOutputCount() const override;
	
	void getInputNames(std::function<void(const std::string* name_p)> inserter) const override;	
	void getOutputNames(std::function<void(const std::string* name_p)> inserter) const override;
	
	bool isInputChannelExists(const std::string& name) const override;
	bool isOutputChannelExists(const std::string& name) const override;
	
	DiscreteState getInputState(const std::string& name) const override;
	
	DiscreteState getOutputState(const std::string& name) const override;
	bool setOutputState(const std::string& name, DiscreteState value) override;
	
	bool localOff() override;
	
	bool initialize(ILogger* logger_p, FS* fileSystem_p, bool initializeDefaults);
	void loop(int timeElapsed_msec);
};