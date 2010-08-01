#ifndef PINS_H
#define PINS_H

class Pins;


class Pins
{
	public:
		/** Set the output pin's level. */
		virtual void write(uint8_t value) = 0;
		
		/** Read the level on the input pin. */
		virtual uint8_t read(void) = 0;
		
		/** Setup pin modes etc. ready for use */
		virtual void setup(void) = 0;
};


class DirectPins : public Pins
{
	protected:
		uint8_t write_pin;
		uint8_t read_pin;
	
	public:
		/**
		 * @param read_pin  Number of the pin the use as input.
		 * @param write_pin Number of the pin the use as output.
		 */
		DirectPins(uint8_t read_pin, uint8_t write_pin);
		
		inline virtual void
		write(uint8_t value)
		{
			digitalWrite(write_pin, value);
		}
		
		inline virtual
		uint8_t read(void)
		{
			return digitalRead(read_pin);
		}
		
		virtual void setup(void);
};


class MultiplePins : public DirectPins
{
	protected:
		uint8_t first_pin;
		uint8_t step;
		uint8_t num_channels;
		uint8_t read_pin_offset;
		uint8_t write_pin_offset;
	
	public:
		MultiplePins(uint8_t first_pin, uint8_t step, uint8_t num_channels,
		             uint8_t read_pin_offset, uint8_t write_pin_offset);
		
		virtual void setup(void);
		
		virtual void set_channel(uint8_t channel);
};

#endif
