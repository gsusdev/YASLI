#include "channels_reader.h"

template<typename T>
bool parseChannelInfo_common(const std::string& line, T& result_out)
{
	size_t pos = 0;
	std::string curWord = line;

	StringUtils::trim(curWord);
	result_out.name = curWord;
	 
	return true;
	
	/*if (!StringUtils::nextWord(line, pos, curWord))
	{
		return false;
	}
	else
	{
		unsigned int number;		
		const auto parsingResult = StringUtils::parseNumber(curWord, number);
		if (parsingResult != NumberParsingResult::success)
		{
			return false;
		}
		
		if (!StringUtils::nextWord(line, pos, curWord))
		{
			return false;
		}
		else
		{
			result_out.number = number;
			result_out.name = curWord;
		}
	}*/
}

template <>
bool ChannelsReader<InputChannelInfo>::parseChannelInfo(const std::string& line, InputChannelInfo& result_out)
{
	const auto ok = parseChannelInfo_common(line, result_out);
	return ok;
}

template <>
bool ChannelsReader<OutputChannelInfo>::parseChannelInfo(const std::string& line, OutputChannelInfo& result_out)
{
	const auto ok = parseChannelInfo_common(line, result_out);
	return ok;
}