#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdbool>
#include <cmath>
#include <cassert>

#include <string>
#include <map>
#include <vector>

#include <algorithm>
#include <memory>

#include "types.h"

using PSettingValue = std::unique_ptr<ISettingValue>;

enum class SettingValueSource
{
	empty,
	defaultValue,
	userValue 
};

bool settingOk(SettingValueSource valueSource);

template <typename T>
class GenericSettingValue : public ISettingValue
{
public:
	std::string toString() const override;
	bool parse(const std::string& value, std::string* errorText_out_p = nullptr) override;
	
	virtual bool isEmpty() const  override { return _isEmpty; }
	
	virtual void clear() override { _isEmpty = true; }
	
	void setDefaultValue(const T& value) { _defaultValue = value; }
	void setIsRequired(bool value) { _isRequired = value; }
	
	std::string getCaption() const override { return _caption; }
	void setCaption(const std::string& value) { _caption = value; }
	
	virtual std::string getValueHint() const override 
	{
		if (_valueHint.empty())
		{
			return getDefaultValueHint();
		}
		
		return _valueHint;
	}
	
	void setValueHint(const std::string& value) { _valueHint = value; }
	
	SettingValueSource get(T& result_out) const 
	{ 
		if (!_isEmpty)
		{
			result_out = _value;
			return SettingValueSource::userValue;
		}
		
		if (_isRequired)
		{
			return SettingValueSource::empty;
		}
		
		result_out = _defaultValue;
		return SettingValueSource::defaultValue;
	}
	
	void set(const T& value)
	{
		_value = value;
		_isEmpty = false;
	}
	
protected:
	virtual std::string getDefaultValueHint() const { return std::string(); }	
	
private:
	T _value;
	T _defaultValue;
	bool _isEmpty = true;
	bool _isRequired = true;
	std::string _caption;
	std::string _valueHint;
};

template <typename T>
class EnumSettingValue : public ISettingValue
	{
	public:
		std::string toString() const override
		{
			if (isEmpty())
			{
				return std::string();
			}
			
			T value;
			if (get(value) != SettingValueSource::userValue)
			{
				return std::string();
			}
			
			const auto it = designators().find(value);
			
			assert(it != designators().end());
			
			const auto& valList = it->second;
			assert(!valList.empty());			
			
			return valList.front();
		}
		
		bool parse(const std::string& value, std::string* errorText_out_p = nullptr) override
		{
			auto text = value;
			
			if (!isCaseSensitive())
			{
				std::transform(text.begin(), text.end(), text.begin(), [](int ch) { return std::tolower(ch); });
			}
						
			for (const auto& pair : designators())
			{
				const auto& valList = pair.second;
				if (valList.empty())
				{
					continue;
				}
				
				auto findResult = std::find(std::begin(valList), std::end(valList), text);
				if (findResult != valList.end())
				{
					set(pair.first);
					return true;
				}
			}
			
			if (errorText_out_p != nullptr)
			{
				*errorText_out_p = "Invalid value";
			}
			
			return false;
		}
		
		virtual bool isEmpty() const  override { return _isEmpty; }
	
		virtual void clear() override { _isEmpty = true; }
	
		void setDefaultValue(const T& value) { _defaultValue = value; }
		void setIsRequired(bool value) { _isRequired = value; }
	
		std::string getCaption() const override { return _caption; }
		void setCaption(const std::string& value) { _caption = value; }
	
		virtual std::string getValueHint() const override 
		{
			if (_valueHint.empty())
			{
				return getDefaultValueHint();
			}
		
			return _valueHint;
		}
	
		void setValueHint(const std::string& value) { _valueHint = value; }
	
		SettingValueSource get(T& result_out) const 
		{ 
			if (!_isEmpty)
			{
				result_out = _value;
				return SettingValueSource::userValue;
			}
		
			if (_isRequired)
			{
				return SettingValueSource::empty;
			}
		
			result_out = _defaultValue;
			return SettingValueSource::defaultValue;
		}
	
		void set(const T& value)
		{
			_value = value;
			_isEmpty = false;
		}
		
	protected:
		virtual const std::map<T, std::vector<std::string>>& designators() const = 0;	
		virtual bool isCaseSensitive() const { return false; }
		
		std::string getDefaultValueHint() const
		{
			const auto length = getDefaultValueHintInternal(nullptr);
			
			std::string s;
			s.reserve(length);
			getDefaultValueHintInternal(&s);
			
			return s;
		}
		
	private:
		size_t getDefaultValueHintInternal(std::string* string_p) const 
		{
			size_t len = 0;
			
			static const std::string delimiter = " | ";
			auto first = true;
			
			for (const auto& designator : designators())
			{
				const auto& valuesList = designator.second;
				if (valuesList.empty())
				{
					continue;
				}
				
				if (!first)
				{
					len += delimiter.size();
					if (string_p != nullptr)
					{
						string_p->append(delimiter);
					}
				}
				
				first = false;
				
				const auto& text = valuesList.front();
				len += text.size();
				if (string_p != nullptr)
				{
					string_p->append(text);
				}
			}
			
			return len;
		}		
		
		T _value;
		T _defaultValue;
		bool _isEmpty = true;
		bool _isRequired = true;
		std::string _caption;
		std::string _valueHint;
	};

using IntegerSettingValue = GenericSettingValue<int>;
using FloatSettingValue = GenericSettingValue<float>;
using StringSettingValue = GenericSettingValue<std::string>;

class BooleanSettingValue : public EnumSettingValue<bool>
{
protected:
	const std::map<bool, std::vector<std::string>>& designators() const override
	{
		static const std::map<bool, std::vector<std::string>> result = 
		{
			{ true,  { "true",  "yes", "on",  "1" } },
			{ false, { "false", "no",  "off", "0" } }
		};
	
		return result;
	}	
};
	
class SettingsGroupBase : public ISettingsGroup
{
public:
	SettingsGroupBase()
		: ISettingsGroup()
	{}
		
	void reset() override;
	
	size_t getItemCount() const override { return _items.size(); }
	const std::string& getItemName(size_t index) const override;
	bool hasItem(const std::string& name) const override;
	
	bool setItemText(const std::string& name, const std::string& value, std::string* errorText_out_p = nullptr) override;
	bool getItemText(const std::string& name, std::string& value_out) const override; 
	
protected:	
	using Items = std::map<std::string, PSettingValue>;
		
	template <class T> 
		T& addItem(const std::string& name)
		{
			auto newItem_p = new T();
			const auto insertResult = _items.insert(std::make_pair(name, PSettingValue(newItem_p)));
			
			assert(insertResult.second);
			
			return reinterpret_cast<T&>(*newItem_p);
		}
	
	template <class T>
		const T& item(const std::string& name) const 
		{
			return reinterpret_cast<const T&>(*_items.at(name));
		}
	
	template <class T>
		T& item(const std::string& name)
		{
			return reinterpret_cast<T&>(*_items.at(name));
		}
	
	const Items& items() const { return _items; }
	Items& items() { return _items; }
	
private:
	Items _items;
};
