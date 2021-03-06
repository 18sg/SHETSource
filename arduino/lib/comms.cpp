#include <Arduino.h>
#include "pins.h"
#include "comms.h"

#ifdef DIRECT_CONN
Comms::Comms(Pins *new_pins)
	: pins(new_pins)
	, buffer_head(0)
	, buffer_tail(0)
{
	// Set the output line to high (idle)
	pins->Write(HIGH);
}


bool
Comms::Available(void)
{
	return (buffer_head != buffer_tail) || (pins->Read() == LOW);
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
	unsigned long time = micros();
	while (pins->Read() != target_state) {
		if ((micros() - time) >= TIMEOUT_CYCLES) {
			return false;
		}
	}
	
	return true;
}


bool
Comms::Write(uint8_t byte)
{
	// Is the remote device already trying to write?
	int bytes_read = 0;
	while (pins->Read() == LOW) {
		if (!ReadToBuffer())
			return false;
		if (++bytes_read >= MAX_PRE_WRITE_READS)
			return false;
	}
	
	
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
	
	Clock();
	Clock();
	Clock();
	Clock();
	Clock();
	
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
	if (buffer_head != buffer_tail || ReadToBuffer()) {
		// Read buffered value
		if (buffer_head == buffer_tail) {
			// Buffer empty!
			return -1;
		}
		
		int data = buffer[buffer_head];
		buffer_head = (buffer_head + 1) % COMMS_BUFFER_SIZE;
		return data;
	}
	return -1;
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


bool
Comms::ReadToBuffer()
{
	// Wait until the remote device acknowledges or the timeout expires.
	if (!WaitTimeout(LOW))
		return false;
	
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
		return false;
	}
	
	// Check buffer isn't too full
	int new_buffer_tail = (buffer_tail + 1) % COMMS_BUFFER_SIZE;
	if (new_buffer_tail == buffer_head) {
		return false;
	}
	
	// Store data in buffer
	buffer[buffer_tail] = data;
	buffer_tail = new_buffer_tail;
	
	return true;
}

#endif
#ifdef SERIAL_CONN

Comms::Comms(long speed)
{
	Serial.begin(speed);
}

bool
Comms::Write(uint8_t byte)
{
	Serial.write(byte);
	return true;
}

bool
Comms::Write(uint8_t *buf, int len)
{
	Serial.write(buf, len);
	return true;
}

bool
Comms::Write(char *str)
{
	Serial.write(str);
	Serial.write((uint8_t)0);
	return true;
}

int
Comms::Read()
{
	// Wait for TIMEOUT_CYCLES micros, or until the serial is available.
	unsigned long time = micros();
	while (!Serial.available()) {
		if (micros() - time >= TIMEOUT_CYCLES)
			return -1;
	}
	
	return Serial.read();
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

bool
Comms::Available(void)
{
	return Serial.available();
}

#endif
#ifdef ENET_CONN

Comms::Comms(EthernetClient *enet_client)
	:enet_client(enet_client)
{
}

bool
Comms::Write(uint8_t byte)
{
	enet_client->write(byte);
	return true;
}

bool
Comms::Write(uint8_t *buf, int len)
{
	enet_client->write(buf, len);
	return true;
}

bool
Comms::Write(char *str)
{
	enet_client->write(str);
	enet_client->write((uint8_t)0);
	return true;
}

int
Comms::Read()
{
	// Wait for TIMEOUT_CYCLES micros, or until the serial is available.
	unsigned long time = micros();
	while (!enet_client->available()) {
		if (micros() - time >= TIMEOUT_CYCLES)
			return -1;
	}
	
	return enet_client->read();
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

bool
Comms::Available(void)
{
	return enet_client->available();
}

#endif
