#include <cstdlib>
#include <cstdio>

#include <string>

#include "stringToNumber.h"
#include "string_utils.h"

bool StringUtils::equal(const std::string &v1, const std::string &v2, bool caseInsensitive)
{
    const auto sz = v1.size();
    
    if (sz != v2.size())
    {
        return false;
    }
    
    if (!caseInsensitive)
    {
        const auto result = v1.compare(v2) == 0;
        return result;
    }
    
    for (size_t i = 0; i < sz; ++i)
    {
        if (std::tolower(v1[i]) != std::tolower(v2[i]))
        {
            return false;
        } 
    }
        
    return true;
}

/*std::string StringUtils::toString(unsigned int value)
{
    const auto& result = std::to_string(value);
    return result;
}*/

std::string StringUtils::toString(int value)
{	
	char buf[24];
	
	int res = snprintf(buf, sizeof(buf), "%d", value);
	if (res < 0)
	{
		return std::string();
	}
	
	std::string str(buf, static_cast<size_t>(res));
	return str;	
}

static NumberParsingResult convertParsingResult(StringToNumResult_t value)
{
	switch (value)
	{
	   	case StringToNumResult_t::stnrOk :
		    return NumberParsingResult::success;
		
		case StringToNumResult_t::stnrSyntaxError :
		    return NumberParsingResult::invalidSyntax;
		
		case StringToNumResult_t::stnrOverflow :
		case StringToNumResult_t::stnrUnderflow :
		    return NumberParsingResult::outOfBounds;
		
		default:
		    return NumberParsingResult::internalError;
	}
}

NumberParsingResult StringUtils::parseNumber(const std::string& text, float& result_out, char decimalSeparator)
{
	const auto res = tryStringToFloat(text.c_str(), text.size(), &result_out, sizeof(result_out), decimalSeparator);
	return convertParsingResult(res);
}
	
NumberParsingResult StringUtils::parseNumber(const std::string& text, int& result_out)
{
	const auto res = tryStringToInt(text.c_str(), text.size(), &result_out, sizeof(result_out));
	return convertParsingResult(res);
}

NumberParsingResult StringUtils::parseNumber(const std::string& text, unsigned int& result_out, unsigned int base)
{
	const auto res = tryStringToUint(text.c_str(), text.size(), &result_out, sizeof(result_out), base);
	return convertParsingResult(res);
}

static const char* ws = " \t\n\r\f\v";

// trim from end of string (right)
std::string& StringUtils::rtrim(std::string& s)
{
    s.erase(s.find_last_not_of(ws) + 1);
    return s;
}

// trim from beginning of string (left)
std::string& StringUtils::ltrim(std::string& s)
{
    s.erase(0, s.find_first_not_of(ws));
    return s;
}

// trim from both ends of string (right then left)
std::string& StringUtils::trim(std::string& s)
{
    return ltrim(rtrim(s));
}

bool StringUtils::nextWord(const std::string &text, size_t &pos, std::string &word_out)
{
    auto isDelimiter = [](char ch)
    {
        return (ch == ' ') || (ch == '\t') || (ch == 0x0D) || (ch == 0x0A);
    };

    const auto textLen = text.size();

    while ((pos < textLen) && isDelimiter(text[pos]))
    {
        ++pos;
    }

    if (pos >= textLen)
    {
        return false;
    }

    const auto wordStart = pos;
    size_t wordLength = 0;

    while ((pos < textLen) && !isDelimiter(text[pos]))
    {
        ++pos;
        ++wordLength;
    }

    word_out = text.substr(wordStart, wordLength);
    return true;
}
