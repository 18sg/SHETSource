#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include "comms.h"
#include "SHETSource.h"


uint8_t mac[] = {0x90, 0xA2, 0xDA, 0x0D, 0x1F, 0x16};
const char *shet_host = "shet-server";
const uint16_t shet_port = 11236;
EthernetClient enet_client;
Comms comms = Comms(&enet_client);

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


/* Ensure that the ethernet client is connected to the server. */
void
ensure_connected(void)
{
	if (!enet_client.connected()) {
		enet_client.stop();
		do {
			delay(2000);
		} while (!enet_client.connect(shet_host, shet_port));
		client.SetState(SHETSource::STATUS_RESETTING);
	}
}


void
setup()
{
	do {
		delay(2000);
	} while (!Ethernet.begin(mac));
	ensure_connected();
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
	
}

void
loop()
{
	ensure_connected();
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
