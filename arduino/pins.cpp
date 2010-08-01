#include "WProgram.h"
#include "pins.h"

DirectPins::DirectPins(uint8_t new_read_pin, uint8_t new_write_pin)
{
	read_pin  = new_read_pin;
	write_pin = new_write_pin;
}

void
DirectPins::Setup()
{
	// Set up the reading pin and activate the pullup
	pinMode(read_pin, INPUT);
	digitalWrite(read_pin, HIGH);
	
	// Setup the writing pin
	pinMode(write_pin, OUTPUT);
}



MultiplePins::MultiplePins(uint8_t new_first_pin,
                           uint8_t new_step,
                           uint8_t new_num_channels,
                           uint8_t new_read_pin_offset,
                           uint8_t new_write_pin_offset) :
DirectPins::DirectPins(0, 0)
{
	first_pin        = new_first_pin;
	step             = new_step;
	num_channels     = new_num_channels;
	read_pin_offset  = new_read_pin_offset;
	write_pin_offset = new_write_pin_offset;
}

void
MultiplePins::SetChannel(uint8_t channel)
{
	read_pin = first_pin + (step*channel) + read_pin_offset;
	write_pin = first_pin + (step*channel) + write_pin_offset;
}

void
MultiplePins::Setup()
{
	uint8_t channel;
	for (channel = 0; channel < num_channels; channel++) {
		SetChannel(channel);
		DirectPins::Setup();
	}
}
