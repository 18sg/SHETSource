#ifndef COMMS_H
#define COMMS_H

#define COMMS_CLOCK_PERIOD 100ul
#define TIMEOUT_CYCLES 100000ul
#define COMMS_BUFFER_SIZE 64
#define MAX_PRE_WRITE_READS COMMS_BUFFER_SIZE


class Comms;

class Comms {
	public:
		Comms(Pins *new_pins);
		
		bool Write(uint8_t byte);
		bool Write(uint8_t *buf, int len);
		bool Write(char *str);
		
		int  Read();
		bool Read(uint8_t *buf, int len);
		
		bool Available(void);
	
	private:
		Pins *pins;
		void Clock(void);
		void HalfClock(void);
		
		bool WaitTimeout(uint8_t target_state);
		
		int     buffer_head;
		int     buffer_tail;
		uint8_t buffer[COMMS_BUFFER_SIZE];
		
		bool ReadToBuffer(void);
};

#endif
