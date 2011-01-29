#include "WProgram.h"
#include "pins.h"
#include "comms.h"
#include "SHETSource.h"


DirectPins pins = DirectPins(2,3);
Comms comms = Comms(&pins);

/* All addresses from this arduino will be prefixed with the string given as the
 * argument below, don't forget the trailing slash if you want it to be a
 * subdirectory! */
SHETSource::Client client = SHETSource::Client(&comms, "testduino");

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
	
	
	/* An action -- increments an integer argument. */
	client.AddAction("doit", doit);
	
	/* An LED as a property (as we can't use 13). */
	pinMode(8, OUTPUT);
	led = HIGH;
	digitalWrite(8, HIGH);
	client.AddProperty("light", set_light, get_light);
	
	/* A Random Variable */
	var = 1337;
	client.AddProperty("var", &var);
	
	/* A button. */
	pinMode(7, INPUT);
	digitalWrite(7, HIGH);
	button_down = false;
	press_event = client.AddEvent("btn");
	
	Serial.begin(9600);
	Serial.print("Hi!");
}

void
loop()
{
	client.DoSHET();
	
	/* Raise the event every time the button is pressed. */
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
