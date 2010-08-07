#include "WProgram.h"
#include "pins.h"
#include "comms.h"

#define SERIAL_SPEED 9600
#define MAX_CLIENTS  2


DirectPins pins = MultiplePins(2,2,MAX_CLIENTS,0,1);
Comms comms = Comms(&pins);

void
setup()
{
	Serial.begin(SERIAL_SPEED);
	
	pins.Init();
	pins.SetChannel(0);
}


void
loop()
{
	// Put your shet here.
}



