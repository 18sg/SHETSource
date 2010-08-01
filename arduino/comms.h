#ifndef COMMS_H
#define COMMS_H

#define COMMS_CLOCK_PERIOD 50
#define TIMEOUT_CYCLES 2000


class Comms;

class Comms {
	private:
		Pins *pins;
		void clock(void);
		void halfClock(void);
		
		bool waitTimeout(uint8_t target_state);
	
	public:
		Comms(Pins *new_pins);
		
		bool write(uint8_t byte);
		bool write(uint8_t *buf, int len);
		
		int read();
		bool read(uint8_t *buf, int len);
		
		bool available(void);
};

#endif
