#include <algorithm>
#include "string_utils.h"

#include "standard_resolvers.h"

const std::map<std::string, EventType> StandardResolvers::_eventTypes =
{
    { "rise", EventType::rise },
    { "fall", EventType::fall }
};

const std::map<std::string, ActionType> StandardResolvers::_actionTypes =
{
    { "turn_on",    ActionType::turnOn    },
    { "turn_off",   ActionType::turnOff   },
    { "toggle",     ActionType::toggle    },
    { "local_off",  ActionType::localOff  },
    { "global_off", ActionType::globalOff }
};

StandardResolvers::StandardResolvers() :
    IEventNameResolver(),
    IActionNameResolver()
{
}

bool StandardResolvers::resolveEventName(const std::string& eventName, EventType& eventType_out) const
{
    const auto result = genericResolve(eventName, _eventTypes, eventType_out);
    return result; 
}

bool StandardResolvers::resolveActionName(const std::string& actionName, ActionType& actionType_out) const
{
    const auto result = genericResolve(actionName, _actionTypes, actionType_out);
    return result;
}

template<typename T>
bool StandardResolvers::genericResolve(const std::string& designator, const std::map<std::string, T>& dictionary, T& result_out)
{
    auto temp = designator;
    temp = StringUtils::trim(temp);

    std::transform(temp.begin(), temp.end(), temp.begin(), [] (int ch) { return std::tolower(ch); });

    const auto it = dictionary.find(designator);
    if (it == dictionary.end())
    {
        return false;
    }

    result_out = it->second;
    return true;
}
