#pragma once

#include <cstdint>
#include <cstddef>
#include <cstdbool>
#include <cassert>

#include <string>

#include "types.h"

#include <Arduino.h>
 
class StreamLogWriter : public ILogWriter
{
public: 
	StreamLogWriter()
		: ILogWriter()
	{}
	
	void setStream(Stream* value_p) { _stream_p = value_p; }
	
	void write(const std::string& text) override
	{
		assert(_stream_p != nullptr);
		_stream_p->write(text.c_str(), text.size());		
	}
	
private:
	Stream* _stream_p = nullptr;
};