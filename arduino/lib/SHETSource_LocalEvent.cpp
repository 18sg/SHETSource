#include <Arduino.h>
#include "pins.h"
#include "comms.h"
#include "SHETSource.h"

using namespace SHETSource;


void
LocalEvent::Add(char *address)
{
	this->address = address;
}


void
LocalEvent::Remove(void)
{
	this->address = NULL;
}


void
LocalEvent::operator () (void)
{
	if (address == NULL) {
		client->ORState(STATUS_MALFORMED_COMMAND);
		return;
	}
	
	if (!( client->WriteCommand(COMMAND_RAISE_EVENT)
	    && client->WriteEventID(&id)
	    && client->WriteType(TYPE_VOID))) {
		/* Something went wrong! */
		return;
	}
}


void
LocalEvent::operator () (int val)
{
	if (address == NULL) {
		client->ORState(STATUS_MALFORMED_COMMAND);
		return;
	}
	
	if (!( client->WriteCommand(COMMAND_RAISE_EVENT)
	    && client->WriteEventID(&id)
	    && client->WriteType(TYPE_INT)
	    && client->WriteInt(&val))) {
		/* Something went wrong! */
		return;
	}
}


void
LocalEvent::Register(void)
{
	if (address != NULL) {
		client->WriteCommand(COMMAND_ADD_EVENT);
		client->WriteEventID(&id);
		client->WriteString(address);
	}
}


void
LocalEvent::Unregister(void)
{
	if (address != NULL) {
		client->WriteCommand(COMMAND_REMOVE_EVENT);
		client->WriteEventID(&id);
	}
}
