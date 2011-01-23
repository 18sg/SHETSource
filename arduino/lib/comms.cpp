#include "WProgram.h"
#include "pins.h"
#include "comms.h"

Comms::Comms(Pins *new_pins):
pins(new_pins)
{
	// Set the output line to high (idle)
	pins->Write(HIGH);
}


bool
Comms::Available(void)
{
	return pins->Read() == LOW;
}


inline void
Comms::Clock(void)
{
	unsigned long start = micros();
	while (micros() - start < COMMS_CLOCK_PERIOD)
		;
}


inline void
Comms::HalfClock(void)
{
	unsigned long start = micros();
	while (micros() - start < COMMS_CLOCK_PERIOD/2)
		;
}


bool
Comms::WaitTimeout(uint8_t target_state)
{
	// Busy-loop until the remote device signals that it is ready or the timeout
	// expires.
	long time = 0;
	while (pins->Read() != target_state) {
		time ++;
		if (time >= TIMEOUT_CYCLES) {
			return false;
		}
	}
	
	return true;
}


bool
Comms::Write(uint8_t byte)
{
	// Signal that we're waiting to write
	pins->Write(LOW);
	
	// Wait until the remote device acknowledges or the timeout expires.
	if (!WaitTimeout(LOW)) {
		pins->Write(HIGH);
		return false;
	}
	
	// Start sending data
	int bit;
	for (bit = 0; bit < 8; bit ++) {
		pins->Write((byte & (1<<bit)) ? HIGH : LOW);
		Clock();
	}
	
	// Wait until the remote device is finished recieving...
	if (!WaitTimeout(HIGH)) {
		pins->Write(HIGH);
		return false;
	}
	
	// Return the line to the idle state as an end bit
	pins->Write(HIGH);
	
	return true;
}


bool
Comms::Write(uint8_t *buf, int len)
{
	int offset;
	for (offset = 0; offset < len; offset ++) {
		if (!Write(*(buf + offset)))
			return false;
	}
	
	return true;
}


bool
Comms::Write(char *str)
{
	int offset = -1;
	do {
		offset ++;
		if (!Write(*(str + offset)))
			return false;
	} while (*(str + offset) != '\0');
	
	return true;
}


int
Comms::Read()
{
	// Wait until the remote device acknowledges or the timeout expires.
	if (!WaitTimeout(LOW))
		return -1;
	
	// Signal that this device is ready
	pins->Write(LOW);
	
	// Wait half a clock cycle so that we're sampling around the middle of the
	// waveform.
	HalfClock();
	
	// Start receiving data
	uint8_t data = 0x00;
	int bit;
	for (bit = 0; bit < 8; bit ++) {
		data |= ((pins->Read() == HIGH) ? 1 : 0) << bit;
		Clock();
	}
	
	// Return the line to the idle state
	pins->Write(HIGH);
	
	// Wait until the remote device is finished sending...
	if (!WaitTimeout(HIGH)) {
		return -1;
	}

	return data;
}


bool
Comms::Read(uint8_t *buf, int len)
{
	int offset;
	int data;
	for (offset = 0; offset < len; offset ++) {
		data = Read();
		if (data != -1) {
			*(buf + offset) = (uint8_t) data;
		} else {
			return false;
		}
	}
	return true;
}
