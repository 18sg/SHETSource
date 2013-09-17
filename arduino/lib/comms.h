#ifndef COMMS_H
#define COMMS_H

#if !(defined SERIAL_CONN || defined ENET_CONN)
#define DIRECT_CONN
#endif

#ifdef ENET_CONN
#include <Ethernet.h>
#endif


#define COMMS_CLOCK_PERIOD 100ul
#define TIMEOUT_CYCLES 500000ul
#define COMMS_BUFFER_SIZE 64
#define MAX_PRE_WRITE_READS COMMS_BUFFER_SIZE


class Comms;

class Comms {
	public:
#ifdef SERIAL_CONN
		Comms(long speed);
#endif
#ifdef ENET_CONN
		Comms(EthernetClient *enet_client);
#endif
#ifdef DIRECT_CONN
		Comms(Pins *new_pins);
#endif
		
		bool Write(uint8_t byte);
		bool Write(uint8_t *buf, int len);
		bool Write(char *str);
		
		int  Read();
		bool Read(uint8_t *buf, int len);
		
		bool Available(void);
	
#ifdef DIRECT_CONN
	private:
		Pins *pins;
		void Clock(void);
		void HalfClock(void);
		
		bool WaitTimeout(uint8_t target_state);
		
		int     buffer_head;
		int     buffer_tail;
		uint8_t buffer[COMMS_BUFFER_SIZE];
		
		bool ReadToBuffer(void);
#endif
#ifdef ENET_CONN
	private:
		EthernetClient *enet_client;
#endif
};

#endif
