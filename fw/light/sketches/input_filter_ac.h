#pragma once

#include <cstdint>
#include <cstddef>
#include <cstdbool>

#include "types.h" 

class InputFilterAc : public IInputFilter
{
  public:
    InputFilterAc();

    void update(DiscreteState value, int timeElapsed_msec) override;
    DiscreteState resultingState() const override { return _currentStableState; }

    int getNominalFrequency_hz() const { return _nominalFrequency_hz; }
    void setNominalFrequency_hz(int value) { _nominalFrequency_hz = value; }

    int getPeriod_msec() const { return _period_msec; }
    void setPeriod_msec(int value) { _period_msec = value; }

  private:
    DiscreteState estimateCurrentState() const;

    DiscreteState _lastRawState;
    int _edgeCount;
    int _edgeCountingDuration_msec;

    DiscreteState _lastStableState;
    DiscreteState _currentStableState;

    int _currentStableDuration_msec;

    int _period_msec;
    int _nominalFrequency_hz;
};
