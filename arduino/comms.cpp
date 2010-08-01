#include "WProgram.h"
#include "pins.h"
#include "comms.h"

Comms::Comms(Pins *new_pins):
pins(new_pins)
{
	// Set the output line to high (idle)
	pins->write(HIGH);
}


bool
Comms::available(void)
{
	return pins->read() == LOW;
}


inline void
Comms::clock(void)
{
	delayMicroseconds(COMMS_CLOCK_PERIOD);
}


inline void
Comms::halfClock(void)
{
	delayMicroseconds(COMMS_CLOCK_PERIOD / 2);
}


bool
Comms::waitTimeout(uint8_t target_state)
{
	// Busy-loop until the remote device signals that it is ready or the timeout
	// expires.
	long time = 0;
	while (pins->read() != target_state) {
		time ++;
		if (time >= TIMEOUT_CYCLES) {
			return false;
		}
	}
	
	return true;
}


bool
Comms::write(uint8_t byte)
{
	// Signal that we're waiting to write
	pins->write(LOW);
	
	// Wait until the remote device acknowledges or the timeout expires.
	if (!waitTimeout(LOW)) {
		pins->write(HIGH);
		return false;
	}
	
	// Start sending data
	int bit;
	for (bit = 0; bit < 8; bit ++) {
		pins->write((byte & (1<<bit)) ? HIGH : LOW);
		clock();
	}
	
	// Wait until the remote device is finished recieving...
	if (!waitTimeout(HIGH)) {
		pins->write(HIGH);
		return false;
	}
	
	// Return the line to the idle state as an end bit
	pins->write(HIGH);
	
	return true;
}


bool
Comms::write(uint8_t *buf, int len)
{
	int offset;
	for (offset = 0; offset < len; offset ++) {
		if (!write(*(buf + offset)))
			return false;
	}
	
	return true;
}


int
Comms::read()
{
	// Wait until the remote device acknowledges or the timeout expires.
	if (!waitTimeout(LOW))
		return -1;
	
	// Signal that this device is ready
	pins->write(LOW);
	
	// Wait half a clock cycle so that we're sampling around the middle of the
	// waveform.
	halfClock();
	
	// Start receiving data
	uint8_t data = 0x00;
	int bit;
	for (bit = 0; bit < 8; bit ++) {
		data |= ((pins->read() == HIGH) ? 1 : 0) << bit;
		clock();
	}
	
	// Return the line to the idle state
	pins->write(HIGH);
	
	// Wait until the remote device is finished sending...
	if (!waitTimeout(HIGH)) {
		return -1;
	}

	return data;
}


bool
Comms::read(uint8_t *buf, int len)
{
	int offset;
	int data;
	for (offset = 0; offset < len; offset ++) {
		data = read();
		if (data != -1) {
			*(buf + offset) = (uint8_t) data;
		} else {
			return false;
		}
	}
	return true;
}
