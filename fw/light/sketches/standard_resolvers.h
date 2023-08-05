#pragma once

#include <cstdint>
#include <cstddef>
#include <cstdbool>

#include <string>
#include <map>

#include "types.h"

class StandardResolvers : public IEventNameResolver, public IActionNameResolver
{
public: 
    StandardResolvers();

    bool resolveEventName(const std::string& eventName, EventType& eventType_out) const override;
    bool resolveActionName(const std::string& actionName, ActionType& actionType_out) const override;

private:
    template <typename T>
    static bool genericResolve(const std::string& designator, const std::map<std::string, T>& dictionary, T& result_out);

    static const std::map<std::string, EventType> _eventTypes;
    static const std::map<std::string, ActionType> _actionTypes;
};
