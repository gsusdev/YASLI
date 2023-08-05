#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdbool>

#include <vector> 
#include <list>

#include "types.h"

class ActionManager : public IActionManager, public IActionManagerConfigurator
{
public:
    ActionManager();

    ActionManagerError setInputEvent(size_t inputChannelIndex, EventType event) override;
    ActionManagerError getOutputState(size_t outputChannelIndex, DiscreteState& outputState_out) override;

    ActionManagerError forceOutput(size_t outputChannelIndex, DiscreteState state) override;
    ActionManagerError forceLocalOff() override;
    ActionManagerError forceGlobalOff() override;

    ActionManagerError isLocalOff(bool& result_out) override;
    ActionManagerError isGlobalOff(bool& result_out) override;

    ActionManagerError execute() override;

    ActionManagerError addRule(const Rule& rule) override;
    ActionManagerError clearRules() override;

    ActionManagerError setInputChannelCount(size_t value) override;
    ActionManagerError setOutputChannelCount(size_t value) override;

private:
    void resetInputForcedStates();
    void resetInputEvents();

    void setAllOff();

    void executeLocalOff();
    void executeGlobalOff();

    void applyRules();
    void applyForcedStates();

    bool isRuleConditionMatches(const RuleCondition& condition) const;
    bool executeRuleAction(const RuleAction& action);

    std::vector<EventType> _curInputEvents;
    std::vector<DiscreteState> _curForcedOutputStates;
    bool _inLocalOff;
    bool _inGlobalOff;

    std::vector<DiscreteState> _curOutputStates;
    bool _outLocalOff;
    bool _outGlobalOff;

    std::list<Rule> _rules;

    static const size_t _maxInputChannelCount = 128;
    static const size_t _maxOutputChannelCount = 128;
    static const size_t _maxRuleCount = 128;
};
