#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdbool>

#include <string>

#include <Arduino.h>

#include "types.h"

void arduinoStringToStdString(const String& arduinoString, std::string& result_out)
{
	result_out = std::move(std::string(arduinoString.c_str()));		
}
 
String macToUuid(const String& mac)
{
	String result = "9f4604de-dc18-11eb-ba80-000000000000";
	
	constexpr auto macStartIndx = 24;
	constexpr auto dstOctetLen = 2;
	constexpr auto srcOctetLen = 3;
	for (auto iOctet = 0; iOctet < 3; iOctet++)
	{
		for (auto iTetrade = 0; iTetrade < 2; iTetrade++)
		{
			result[macStartIndx + iOctet * dstOctetLen + iTetrade] = mac[iOctet * 3 + iTetrade];
		}
	}
	
	return result;		
}