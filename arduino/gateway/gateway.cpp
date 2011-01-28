#include "WProgram.h"
#include "pins.h"
#include "comms.h"

#define STATUS_LED 13
#define NUM_CLIENTS 5
#define SERIAL_SPEED 19200

bool out_of_sync;

void setup(void);
void loop(void);
void OnReadError(int client_address);
void OnWriteError(int client_address);
void RouteDataFromClients(void);
void RouteDataFromServer(void);


MultiplePins pins = MultiplePins(2,2,NUM_CLIENTS,0,1);
Comms comms = Comms(&pins);


void
sync_gateway(void)
{
	//Serial.write("\xFF\xFF\xF0");
	out_of_sync = true;
}


void
setup(void)
{
	Serial.begin(SERIAL_SPEED);
	sync_gateway();
	
	pins.Init();
	pins.SetChannel(0);
	
	pinMode(STATUS_LED, OUTPUT);
	digitalWrite(STATUS_LED, HIGH);
	
	out_of_sync = false;
}


void
loop(void)
{
	RouteDataFromClients();
	RouteDataFromServer();
}


void
OnReadError(int client_address)
{
	// Do Something!
	sync_gateway();
}


void
OnWriteError(int client_address)
{
	// Do Something!
	sync_gateway();
}


void
RouteDataFromClients(void)
{
	uint8_t client_address;
	int data;
	for (client_address = 0; client_address < NUM_CLIENTS; client_address++) {
		pins.SetChannel(client_address);
		
		if (comms.Available()) {
			data = comms.Read();
			
			if (data != -1) {
				/* Forward any data received to the server. */
				Serial.write(client_address);
				Serial.write(0xF0 | (data & 0x0F));
				Serial.write(0xB0 | ((data & 0xF0) >> 4));
			} else {
				OnReadError(client_address);
			}
		}
	}
}


void
RouteDataFromServer(void)
{
	uint8_t client_address, data1, data2;
	
	if (Serial.available() >= 3) {
		// Read the address
		client_address = Serial.read();
		if (client_address & 0x80)
			return;
		
		// Read part 1 of the data
		data1 = Serial.read();
		if ((data1 & 0xF0) != 0xF0)
			return;
		data1 &= 0x0F;
		
		// Read part 2 of the data
		data2 = Serial.read();
		if ((data2 & 0xF0) != 0xB0)
			return;
		data2 &= 0x0F;
		
		// Forward data on to device.
		pins.SetChannel(client_address);
		if (!comms.Write(data1 | (data2 << 4))) {
			OnWriteError(client_address);
		}
	}
}
