#include "WProgram.h"
#include "Comms.h"
#include "SHETSource.h"

using namespace SHETSource;


void
LocalProperty::Add(static char *address,
                   void (*set_callback)(int value),
                   int  (*get_callback)(void))
{
	this->address = address;
	this->get_callback = get_callback;
	this->set_callback = set_callback;
	this->var = NULL;
}


void
LocalProperty::Add(static char *address, int *var)
{
	this->address = address;
	this->get_callback = NULL;
	this->set_callback = NULL;
	this->var = var;
}


void
LocalProperty::Set(Client *client)
{
	if (address == NULL) {
		client->ORState(STATUS_MALFORMED_COMMAND);
		return;
	}
	int new_var;
	if (set_callback != NULL) {
		client->ReadInt(&new_var);
		set_callback(new_var);
	} else if (var != NULL) {
		client->ReadInt(var);
	} else {
		/* Shouldn't get here (tm) */
	}
}


void
LocalProperty::Get(void)
{
	if (address == NULL) {
		client->ORState(STATUS_MALFORMED_COMMAND);
		return;
	}
	int new_var;
	if (set_callback != NULL) {
		new_var = get_callback();
		client->WriteInt(&new_var);
	} else if (var != NULL) {
		client->ReadInt(var);
	} else {
		/* Shouldn't get here (tm) */
	}
}


void
LocalProperty::Register(void)
{
	if (address != NULL) {
		client->WriteCommand(COMMAND_ADD_PROPERTY);
		client->WritePropertyID(&id);
		client->WriteString(address);
		client->WriteType(TYPE_INT);
	}
}


void
LocalProperty::Unregister(void)
{
	if (address != NULL) {
		client->WriteCommand(COMMAND_REMOVE_PROPERTY);
		client->WritePropertyID(&id);
	}
}
