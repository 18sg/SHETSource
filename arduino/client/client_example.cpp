#include "WProgram.h"
#include "pins.h"
#include "comms.h"
#include "SHETSource.h"


DirectPins pins = DirectPins(2,3);
Comms comms = Comms(&pins);
SHETSource::Client client = SHETSource::Client(&comms, "test");

uint8_t data;
uint8_t led;

void
setup()
{
	pins.Init();
	client.Init();
}

void
loop()
{
	client.DoSHET();
	
	// Put your shet here.
}
