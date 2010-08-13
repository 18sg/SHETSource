#include "WProgram.h"
#include "pins.h"
#include "comms.h"
#include "SHETSource.h"


DirectPins pins = DirectPins(2,3);
Comms comms = Comms(&pins);
SHETSource::Client client = SHETSource::Client(&comms, "/test/");

SHETSource::LocalEvent *press_event;

int var;
uint8_t led;
bool button_down;


void
set_light(int value)
{
	digitalWrite(8, value);
	led = value;
}


int get_light(void)
{
	return led;
}


int
doit(int value)
{
	return ++value;
}


void
setup()
{
	pins.Init();
	client.Init();
	
	pinMode(7, INPUT);
	digitalWrite(7, HIGH);
	
	pinMode(8, OUTPUT);
	led = HIGH;
	digitalWrite(8, HIGH);
	
	var = 1337;
	button_down = false;
	
	client.AddAction("doit", doit);
	client.AddProperty("light", set_light, get_light);
	client.AddProperty("var", &var);
	press_event = client.AddEvent("btn");
}

void
loop()
{
	client.DoSHET();
	
	uint8_t button = digitalRead(7);
	if (button == LOW) {
		if (!button_down) {
			button_down = true;
			(*press_event)(1337);
		}
	} else {
		button_down = false;
	}
}
