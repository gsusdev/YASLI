// Based on https://github.com/jmparatte/jm_PCF8574

#ifndef jm_PCF8574_h
#define jm_PCF8574_h

// PCF8574 - PCF8574A
// Remote 8-bit I/O expander for I2C-bus with interrupt

// http://www.ti.com/lit/ml/scyb031/scyb031.pdf
// http://www.ti.com/lit/ds/symlink/pcf8574.pdf
// https://www.nxp.com/docs/en/data-sheet/PCF8574_PCF8574A.pdf

// PCF8574 I2C address: 0x20..0x27
// PCF8574A I2C address: 0x38..0x3F

// Quasi-bidirectional I/Os: P0..P7

#include <stddef.h>
#include <stdint.h>

class jm_PCF8574
{
private:

protected:
	bool _isBuffered = false;
	uint8_t _buffer = -1;
	
	uint8_t	_i2c_address;		// Device I2C address
	bool	_connected;			// Device ready and not errored

	uint8_t _io_mask;			// Digital I/O mask P0-P7
	uint8_t	_io_data;			// Digital I/O data P0-P7

public:

	jm_PCF8574();
	jm_PCF8574(uint8_t i2c_address);
	operator bool();
	
	bool isBuffered() const;
	void setIsBuffered(bool value);

	uint8_t i2c_address() const;
	bool connected() const;

	bool begin();
	bool begin(uint8_t i2c_address);
	void end();
	
	bool readAll();
	bool writeAll();

	int read();
	size_t write(uint8_t value);
	size_t write(const uint8_t *data, size_t quantity);

	uint8_t pin_Mask(uint8_t pin);
	uint8_t pin_Mode(uint8_t pin);
	void pin_Mode(uint8_t pin, uint8_t mode);
	int digital_Read(uint8_t pin);
	int digital_Read(uint8_t pin) const;
	void digital_Write(uint8_t pin, uint8_t value);

	void wait(uint16_t us);

	static void obj_pinMode(void * obj, uint8_t pin, uint8_t mode);
	static int obj_digitalRead(void * obj, uint8_t pin);
	static void obj_digitalWrite(void * obj, uint8_t pin, uint8_t value);
};

#endif // jm_PCF8574_h
