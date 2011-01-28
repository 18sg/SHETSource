#ifndef COMMS_H
#define COMMS_H

#define COMMS_CLOCK_PERIOD 100ul
#define TIMEOUT_CYCLES 10000ul


class Comms;

class Comms {
	private:
		Pins *pins;
		void Clock(void);
		void HalfClock(void);
		
		bool WaitTimeout(uint8_t target_state);
	
	public:
		Comms(Pins *new_pins);
		
		bool Write(uint8_t byte);
		bool Write(uint8_t *buf, int len);
		bool Write(char *str);
		
		int  Read();
		bool Read(uint8_t *buf, int len);
		
		bool Available(void);
};

#endif
