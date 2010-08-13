#include "WProgram.h"
#include "pins.h"
#include "comms.h"

#define NUM_CLIENTS 1
#define SERIAL_SPEED 9600


void setup(void);
void loop(void);
void OnReadError(int client_address);
void OnWriteError(int client_address);
void RouteDataFromClients(void);
void RouteDataFromServer(void);


MultiplePins pins = MultiplePins(2,2,NUM_CLIENTS,0,1);
Comms comms = Comms(&pins);


void
setup(void)
{
	Serial.begin(SERIAL_SPEED);
	
	pins.Init();
	pins.SetChannel(0);
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
}


void
OnWriteError(int client_address)
{
	// Do Something!
}


void
RouteDataFromClients(void)
{
	uint8_t client_address;
	int data;
	for (client_address = 0; client_address < NUM_CLIENTS; client_address++) {
		pins.SetChannel(client_address);
		
		while (comms.Available()) {
			data = comms.Read();
			
			if (data != -1) {
				/* Forward any data received to the server. */
				if (data != -1) {
					Serial.write(client_address);
					Serial.write(data);
				}
			} else {
				OnReadError(client_address);
			}
		}
	}
}


void
RouteDataFromServer(void)
{
	uint8_t client_address, data;
	
	while (Serial.available() >= 2) {
		client_address = Serial.read();
		data = Serial.read();
		
		/* Forward data on to device. */
		pins.SetChannel(client_address);
		if (!comms.Write(data)) {
			OnWriteError(client_address);
		}
	}
}
