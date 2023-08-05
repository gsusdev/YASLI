#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdbool>
#include <cassert>

#include <string>
#include <map>
#include <algorithm>

#include <Arduino.h>
#include <FS.h>

#include "string_utils.h"
#include "types.h"
#include "channels.h" 
#include "log_utils.h"

template <typename T>
class ChannelsReader
{
public:
	void setLogger(ILogger* value_p) { _logger.setLogger(value_p); }
	
	void setFilename(String value) { _filename = value; }
	void setFileSystem(FS* value_p) { _fileSystem_p = value_p; }
	
	void setChannelList(ChannelList<T>* value_p) { _channelList_p = value_p; }
	
	bool execute()
	{
		assert(_fileSystem_p != nullptr);
		assert(_channelList_p != nullptr);
		
		auto file = _fileSystem_p->open(_filename);
		
		if (!file)
		{
			LogError msg(_logger, ILogger::ErrorSeverity::error);
			
			msg.addText("Failed to open file ");
			msg.addText(std::string(_filename.c_str()));
			
			return false;
		}
		
		size_t channelIndex = 0;
		
		while (file.available() > 0)
		{
			auto line = file.readStringUntil('\n');
			std::string s(line.c_str());
			
			StringUtils::trim(s);
			
			if (s.empty())
			{
				continue;
			}
			
			if (s.size() < 2)
			{
				writeInvalidChannelToLog(s);				
				continue;
			}
			
			if (s.substr(0, 2) == "//")
			{
				continue;
			}
			
			T channelInfo;
			if (!parseChannelInfo(s, channelInfo))
			{
				writeInvalidChannelToLog(s);
				continue;
			}
			
			channelInfo.number = channelIndex;
			++channelIndex;
			
			_channelList_p->addChannel(channelInfo);
		}
		
		file.close();
		return true;
	}	
	
private:
	bool parseChannelInfo(const std::string& line, T& result_out);
	
	void writeInvalidChannelToLog(const std::string& s)
	{
		LogError msg(_logger, ILogger::ErrorSeverity::warning);
			
		msg.addText("Invalid channel parameter string: ");
		msg.addText(s);
	}
	
	LoggerHelper _logger;
	
	String _filename;
	FS* _fileSystem_p = nullptr;
	
	ChannelList<T>* _channelList_p = nullptr;
};
	