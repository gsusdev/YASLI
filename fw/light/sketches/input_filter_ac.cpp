#include "input_filter_ac.h"

InputFilterAc::InputFilterAc() :
    IInputFilter(),
   _lastRawState(DiscreteState::unknown),
   _edgeCount(0),
   _edgeCountingDuration_msec(0),
   _lastStableState(DiscreteState::unknown),
   _currentStableState(DiscreteState::unknown),
   _currentStableDuration_msec(0),
   _period_msec(75),
   _nominalFrequency_hz(50) 
{      
}

DiscreteState InputFilterAc::estimateCurrentState() const
{
    const auto edgesPerSecond = _edgeCount * 1000 / _edgeCountingDuration_msec;
    const auto nominalEdgesPerSecond = _nominalFrequency_hz * 2;

    if (edgesPerSecond == 0)
    {
        return DiscreteState::off;
    }

    if (edgesPerSecond > nominalEdgesPerSecond * 3 / 4)
    {
        return DiscreteState::on;
    }

    return DiscreteState::unknown;
}

void InputFilterAc::update(DiscreteState value, int timeElapsed_msec)
{
    if (_lastRawState == DiscreteState::unknown)
    {
        _lastRawState = value;
        _currentStableDuration_msec = 0;
        _currentStableState = DiscreteState::unknown;

        return;
    }

    if ((value != DiscreteState::unknown) && (value != _lastRawState))
    {
        ++_edgeCount;        
    }

    _edgeCountingDuration_msec += timeElapsed_msec;
    _currentStableDuration_msec += timeElapsed_msec;

    if (_edgeCountingDuration_msec >= _period_msec)
    {                
        const auto tempState = estimateCurrentState();
        if (tempState != DiscreteState::unknown)
        {
            _currentStableState = tempState;
      
            if (_lastStableState != _currentStableState)
            {
                _currentStableDuration_msec = 0;
                _lastStableState = _currentStableState;                
            }
        }
        
        _edgeCountingDuration_msec = 0;
        _edgeCount = 0;
    }
    
    _lastRawState = value;
}
