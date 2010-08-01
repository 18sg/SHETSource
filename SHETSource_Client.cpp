#include "WProgram.h"
#include "Comms.h"
#include "SHETSource.h"

using namespace SHETSource;


Client::Client(Comms *comms) :
comms(comms)
{
	action_id_t a;
	for (a = 0; a < NUM_ACTIONS; a++)
		actions[a]->Init(this, id);
	
	event_id_t e;
	for (e = 0; e < NUM_EVENTS; e++)
		events[e]->Init(this, id);
	
	property_id_t p;
	for (p = 0; p < NUM_PROPERTIES; p++)
		properties[p]->Init(this, id);
	
	status_led = 13;
	pinMode(status_led, OUTPUT);
}


/* Communicating with the server **********************************************/


bool
Client::Sync(void)
{
	if (comms->available()) {
		HandleCommands();
	}
	
	if (status & STATUS_BOOTING) {
		InitialiseWithServer();
	}
	
	UpdateStatusLED();
}


void
UpdateStatusLED(void)
{
	uint8_t blink_pattern;
	
	switch (state) {
		case STATUS_CONNECTED       : blink_pattern = 0xFF; /* 11111111 */ break;
		case STATUS_BOOTING         : blink_pattern = 0xAA; /* 10101010 */ break;
		case STATUS_UNKNOWN_COMMAND : blink_pattern = 0xA0; /* 10100000 */ break;
	}
	
	int step = ((millis() / 100) % 8);
	digitalWrite(status_led, (blink_pattern & (1<<step)) ? HIGH : LOW);
}


/* Fetch next available IDs ***************************************************/


action_id_t
Client::GetNextActionID(void)
{
	action_id_t i;
	for (i = 0; i < NUM_ACTIONS; i++)
		if (!actions[i]->InUse())
			return i;
	
	return -1;
}


event_id_t
Client::GetNextEventID(void)
{
	event_id_t i;
	for (i = 0; i < NUM_EVENTS; i++)
		if (!events[i]->InUse())
			return i;
	
	return -1;
}


property_id_t
Client::GetNextPropertyID(void)
{
	property_id_t i;
	for (i = 0; i < NUM_PROPERTIES; i++)
		if (!properties[i]->InUse())
			return i;
	
	return -1;
}


/* Add/Remove Actions *********************************************************/


LocalAction *
Client::AddAction(static char *address, void (*callback)(void))
{
	action_id_t id = GetNextActionID();
	ASSERT(id > 0);
	
	actions[id].Setup(address, callback);
	
	return &(actions[id]);
}


LocalAction *
Client::AddAction(static char *address, void (*callback)(int value))
{
	action_id_t id = GetNextActionID();
	ASSERT(id > 0);
	
	actions[id].Setup(address, callback);
	
	return &(actions[id]);
}


LocalAction *
Client::AddAction(static char *address, int (*callback)(void))
{
	action_id_t id = GetNextActionID();
	ASSERT(id > 0);
	
	actions[id].Setup(address, callback);
	
	return &(actions[id]);
}


LocalAction *
Client::AddAction(static char *address, int (*callback)(int value))
{
	action_id_t id = GetNextActionID();
	ASSERT(id > 0);
	
	actions[id].Setup(address, callback);
	
	return &(actions[id]);
}

void
Client::RemoveAction(LocalAction *action)
{
	action->Remove();
}


/* Add/Remove Events **********************************************************/


LocalEvent *
Client::AddEvent(static char *address)
{
	event_id_t id = GetNextEventID();
	ASSERT(id > 0);
	
	events[id].Setup(address);
	
	return &(events[id]);
}


void
Client::RemoveEvent(LocalEvent *event)
{
	event->Remove();
}


/* Add/Remove Properties ******************************************************/


LocalProperty *
Client::AddProperty(static char *address,
                    void (*set_callback)(int value),
                    int  (*get_callback)(void))
{
	property_id_t id = GetNextPropertyID();
	ASSERT(id > 0);
	
	properties[id].Setup(address, get_callback, set_callback);
	
	return &(events[id]);
}


void
Client::RemoveProperty(LocalProperty *property)
{
	property->Remove();
}

