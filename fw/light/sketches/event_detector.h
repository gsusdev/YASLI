#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdbool>

#include <vector>

#include "types.h"

class EventDetector : public IEventDetector, public IEventDetectorConfigurator
{
public:
    EventDetector(); 

    EventDetectorError setInputState(size_t channelIndex, DiscreteState value) override;
    EventDetectorError getOutputEvent(size_t channelIndex, EventType& result_out) override;

    EventDetectorError setChannelCount(size_t value) override;

    EventDetectorError execute() override;

private:
    using States = std::vector<DiscreteState>;

    void resetPreviousStates();
    void resetEvents();

    States _statesA;
    States _statesB;

    States* _previousStates_p;
    States* _currentStates_p;

    std::vector<EventType> _currentEvents;

    static const size_t _maxChannelCount = 128;
};
