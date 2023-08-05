#include "event_detector.h"

EventDetector::EventDetector() :
    IEventDetector(),
    IEventDetectorConfigurator()
{
    _previousStates_p = &_statesA;
    _currentStates_p = &_statesB; 
}

EventDetectorError EventDetector::setInputState(size_t channelIndex, DiscreteState value)
{
    if (channelIndex >= _currentStates_p->size())
    {
        return EventDetectorError::invalidChannelIndex;
    }

    (*_currentStates_p)[channelIndex] = value;

    return EventDetectorError::none;
}

EventDetectorError EventDetector::getOutputEvent(size_t channelIndex, EventType& result_out)
{
    if (channelIndex >= _currentEvents.size())
    {
        return EventDetectorError::invalidChannelIndex;
    }

    result_out = _currentEvents[channelIndex];

    return EventDetectorError::none;
}

EventDetectorError EventDetector::setChannelCount(size_t value)
{
    if (value > _maxChannelCount)
    {
        return EventDetectorError::tooManyChannels;
    }

    _statesA.resize(value);
    _statesB.resize(value);
    _currentEvents.resize(value);

    resetPreviousStates();
    resetEvents();

    return EventDetectorError::none;
}

EventDetectorError EventDetector::execute()
{
    for (size_t i = 0; i < _currentEvents.size(); ++i)
    {
        const auto prevState = (*_previousStates_p)[i];
        const auto curState = (*_currentStates_p)[i];

        EventType event;

        if ((prevState == DiscreteState::off) && (curState == DiscreteState::on))
        {
            event = EventType::rise;
        }
        else if ((prevState == DiscreteState::on) && (curState == DiscreteState::off))
        {
            event = EventType::fall;
        }
        else
        {
            event = EventType::none;
        }

        _currentEvents[i] = event;
    }

    std::swap(_previousStates_p, _currentStates_p);

    return EventDetectorError::none;
}

void EventDetector::resetPreviousStates()
{
    for (auto it = _previousStates_p->begin(); it != _previousStates_p->end(); ++it)
    {
        *it = DiscreteState::off;
    }
}

void EventDetector::resetEvents()
{
    for (auto it = _currentEvents.begin(); it != _currentEvents.end(); ++it)
    {
        *it = EventType::none;
    }
}
