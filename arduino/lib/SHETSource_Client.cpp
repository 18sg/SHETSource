#include "WProgram.h"
#include "pins.h"
#include "comms.h"
#include "SHETSource.h"

using namespace SHETSource;


Client::Client(Comms *comms, char *address) :
comms(comms),
address(address)
{
	action_id_t a;
	for (a = 0; a < NUM_ACTIONS; a++)
		actions[a].Init(this, a);
	
	event_id_t e;
	for (e = 0; e < NUM_EVENTS; e++)
		events[e].Init(this, e);
	
	property_id_t p;
	for (p = 0; p < NUM_PROPERTIES; p++)
		properties[p].Init(this, p);
	
	SetState(STATUS_RESETTING);
}


void
Client::Init(void)
{
	status_led = 13;
	pinMode(status_led, OUTPUT);
	last_ping = millis();
}


/* Communicating with the server **********************************************/


void
Client::DoSHET(void)
{
	/* Do not return control to the program until the reset has completed. */
	do {
		MainLoop();
	} while (GetState() & STATUS_RESETTING);
}


void
Client::MainLoop(void)
{
	if (GetState() & STATUS_RESETTING) {
		InitialiseWithServer();
	} else {
		HandleRequests();
	}
	
	if (millis() - last_ping > PING_INTERVAL) {
		WriteCommand(COMMAND_PING);
		last_ping = millis();
	}
	
	UpdateStatusLED();
}


void
Client::HandleRequests(void)
{
	if (comms->Available()) {
		last_ping = millis();
		
		command_t command;
		if (!ReadCommand(&command)) return;
		
		switch (command) {
			case COMMAND_RESET        : OnRcvReset();       break;
			case COMMAND_PING         : OnRcvPing();        break;
			case COMMAND_CALL_ACTION  : OnRcvCallAction();  break;
			case COMMAND_SET_PROPERTY : OnRcvSetProperty(); break;
			case COMMAND_GET_PROPERTY : OnRcvGetProperty(); break;
			
			default:
				ORState(STATUS_MALFORMED_COMMAND);
				break;
		}
	}
}


void
Client::InitialiseWithServer(void)
{
	/* If the remote end is trying to write then we can't send a reset. */
	digitalWrite(status_led, LOW);
	while (comms->Available()) {
		while (comms->Available()) {
			// Flush the waiting read data
			comms->Read();
			delay(1);
		}
		delay(100);
	}
	
	// Send a reset
	WriteCommand(COMMAND_RESET);
	WriteString(address);
	
	// Send registration commands
	action_id_t a;
	for (a = 0; a < NUM_ACTIONS; a++)
		actions[a].Register();
	
	event_id_t e;
	for (e = 0; e < NUM_EVENTS; e++)
		events[e].Register();
	
	property_id_t p;
	for (p = 0; p < NUM_PROPERTIES; p++)
		properties[p].Register();
	
	// Everything is working again now, reset the status bitfield
	SetState(STATUS_CONNECTED);
	last_ping = millis();
}


/* Status display *************************************************************/


status_t
Client::GetState(void)
{
	return state;
}


void
Client::SetState(status_t val)
{
	state = val;
}


void
Client::ORState(status_t val)
{
	state |= val;
}


void
Client::UpdateStatusLED(void)
{
	uint8_t blink_pattern;
	
	switch (GetState()) {
		case STATUS_MALFORMED_COMMAND : blink_pattern = 0xAF; /* 10101111 */ break;
		case STATUS_RESETTING         : blink_pattern = 0xAA; /* 10101010 */ break;
		case STATUS_HIGH_LATENCY      : blink_pattern = 0xF0; /* 00001111 */ break;
		case STATUS_CONNECTED         : blink_pattern = 0xFF; /* 11111111 */ break;
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
		if (!actions[i].InUse())
			return i;
	
	return -1;
}


event_id_t
Client::GetNextEventID(void)
{
	event_id_t i;
	for (i = 0; i < NUM_EVENTS; i++)
		if (!events[i].InUse())
			return i;
	
	return -1;
}


property_id_t
Client::GetNextPropertyID(void)
{
	property_id_t i;
	for (i = 0; i < NUM_PROPERTIES; i++)
		if (!properties[i].InUse())
			return i;
	
	return -1;
}


/* Add/Remove Actions *********************************************************/


LocalAction *
Client::AddAction(char *address, void (*callback)(void))
{
	action_id_t id = GetNextActionID();
	ASSERT(id > 0);
	
	actions[id].Add(address, callback);
	
	return &(actions[id]);
}


LocalAction *
Client::AddAction(char *address, void (*callback)(int value))
{
	action_id_t id = GetNextActionID();
	ASSERT(id > 0);
	
	actions[id].Add(address, callback);
	
	return &(actions[id]);
}


LocalAction *
Client::AddAction(char *address, int (*callback)(void))
{
	action_id_t id = GetNextActionID();
	ASSERT(id > 0);
	
	actions[id].Add(address, callback);
	
	return &(actions[id]);
}


LocalAction *
Client::AddAction(char *address, int (*callback)(int value))
{
	action_id_t id = GetNextActionID();
	ASSERT(id > 0);
	
	actions[id].Add(address, callback);
	
	return &(actions[id]);
}

void
Client::RemoveAction(LocalAction *action)
{
	action->Remove();
}


/* Add/Remove Events **********************************************************/


LocalEvent *
Client::AddEvent(char *address)
{
	event_id_t id = GetNextEventID();
	ASSERT(id > 0);
	
	events[id].Add(address);
	
	return &(events[id]);
}


void
Client::RemoveEvent(LocalEvent *event)
{
	event->Remove();
}


/* Add/Remove Properties ******************************************************/


LocalProperty *
Client::AddProperty(char *address,
                    void (*set_callback)(int value),
                    int  (*get_callback)(void))
{
	property_id_t id = GetNextPropertyID();
	ASSERT(id > 0);
	
	properties[id].Add(address, set_callback, get_callback);
	
	return &(properties[id]);
}

LocalProperty *
Client::AddProperty(char *address,
                    int  *var)
{
	property_id_t id = GetNextPropertyID();
	ASSERT(id > 0);
	ASSERT(var != NULL);
	
	properties[id].Add(address, var);
	
	return &(properties[id]);
}


void
Client::RemoveProperty(LocalProperty *property)
{
	property->Remove();
}

