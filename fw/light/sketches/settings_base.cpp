#include <cstdio>

#include "string_utils.h"
#include "settings_base.h"

bool settingOk(SettingValueSource valueSource)
{
	return (valueSource == SettingValueSource::defaultValue) || (valueSource == SettingValueSource::userValue);
}

template<>
std::string GenericSettingValue<std::string>::toString() const
{
	return _value;
}

template<>
	bool GenericSettingValue<std::string>::parse(const std::string& value, std::string* errorText_out_p)
{
	_value = value;
	_isEmpty = false;
	 
	return true;
}

template<>
	bool GenericSettingValue<int>::parse(const std::string& value, std::string* errorText_out_p)
{
	const auto res = StringUtils::parseNumber(value, _value);
	
	if (res == NumberParsingResult::success)
	{
		_isEmpty = false;
		return true;
	}
	
	return false;
}

template<>
std::string GenericSettingValue<int>::toString() const
{
	if (_isEmpty)
	{
		return std::string();
	}
	
	char buffer[24];	
	const auto len = snprintf(buffer, sizeof(buffer), "%d", _value);
	if (len <= 0)
	{
		return std::string();
	}
	
	return std::string(buffer, len);
}

template<>
	bool GenericSettingValue<float>::parse(const std::string& value, std::string* errorText_out_p)
{
	const auto res = StringUtils::parseNumber(value, _value, '.');
	
	if (res == NumberParsingResult::success)
	{
		_isEmpty = false;
		return true;
	}
	
	return false;
}

template<>
std::string GenericSettingValue<float>::toString() const
{
	if (_isEmpty)
	{
		return std::string();
	}
	
	char buffer[64];	
	const auto len = snprintf(buffer, sizeof(buffer), "%f", _value);
	if (len <= 0)
	{
		return std::string();
	}
	
	return std::string(buffer, len);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SettingsGroupBase::reset()
{
	for (auto& item : items())
	{
		item.second->clear();
	}
}
	
const std::string& SettingsGroupBase::getItemName(size_t index) const
{
	static const std::string emptyString;
	
	if (index >= items().size())
	{
		return emptyString;
	}
	
	auto it = items().begin(); 
	std::advance(it, index);
	
	return it->first;
}

bool SettingsGroupBase::setItemText(const std::string& name, const std::string& value, std::string* errorText_out_p)
{
	auto it = _items.find(name);
	if (it == _items.end())
	{
		if (errorText_out_p != nullptr)
		{
			*errorText_out_p = "Invalid parameter name";			
		}
		
		return false;
	}
	
	const auto result = it->second->parse(value, errorText_out_p);
	return result;
}

bool SettingsGroupBase::getItemText(const std::string& name, std::string& value_out) const
{
	auto it = _items.find(name);
	if (it == _items.end())
	{
		return false;
	}
	
	value_out = it->second->toString();	
	return true;
}

bool SettingsGroupBase::hasItem(const std::string& name) const
{
	const auto it = _items.find(name);
	return it != _items.end();
}
