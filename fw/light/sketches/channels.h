#pragma once

#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdbool>

#include <string>
#include <map>
#include <algorithm>

#include "types.h" 

struct InputChannelInfo
{
	std::string name;
	size_t number;
};

struct OutputChannelInfo
{
	std::string name;
	size_t number;
};

template <typename T>
	class ChannelList : public IChannelNameResolver
	{
	public:
		ChannelList()
			: IChannelNameResolver()
		{
		}
	
		bool resolveChannelName(const std::string& channelName, size_t& channelIndex_out) const override
		{
			auto name = channelName;
			std::transform(name.begin(), name.end(), name.begin(), [](int ch) { return std::tolower(ch); });
		
			for (const auto& item : _channels)
			{
				if (item.second.name == name)
				{
					channelIndex_out = item.first;
					return true;
				}
			}
		
			return false;
		}
	
		bool getChannelName(size_t index, std::string& result_out) const
		{
			const auto it = _channels.find(index);
			if (it == _channels.end()) 
			{
				return false;
			}
			
			result_out = it->second.name;
			return true;
		}
		
		void addChannel(const T& channel)
		{
			_channels[channel.number] = channel;
		
			auto& name = _channels[channel.number].name;
			std::transform(name.begin(), name.end(), name.begin(), [](int ch) { return std::tolower(ch); });
		}
	
	private:
		std::map<size_t, T> _channels;
	};

using InputChannelList = ChannelList<InputChannelInfo>;
using OutputChannelList = ChannelList<OutputChannelInfo>;	