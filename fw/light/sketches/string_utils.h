#pragma once

#include <cstdint>
#include <cstddef>
#include <cstdbool>

#include <string>

enum class NumberParsingResult
{
	success,
	invalidSyntax,
	outOfBounds, 
	internalError
};

class StringUtils
{
public:	
    static bool equal(const std::string &v1, const std::string &v2, bool caseInsensitive = true);
    
    /*static std::string toString(unsigned int value);*/
    static std::string toString(int value);
	
	static NumberParsingResult parseNumber(const std::string& text, float& result_out, char decimalSeparator = '.');
	static NumberParsingResult parseNumber(const std::string& text, int& result_out);
	static NumberParsingResult parseNumber(const std::string& text, unsigned int& result_out, unsigned int base = 10);

    static std::string& rtrim(std::string& s);
    static std::string& ltrim(std::string& s);
    static std::string& trim(std::string& s);

    static bool nextWord(const std::string &text, size_t &pos, std::string &word_out);

};
