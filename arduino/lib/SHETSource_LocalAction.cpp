#include <Arduino.h>
#include "pins.h"
#include "comms.h"
#include "SHETSource.h"

namespace SHETSource{

void
LocalAction::Add(char *address, void (*callback)(void))
{
	this->address = address;
	this->callback_vv = callback;
	this->callback_vi = NULL;
	this->callback_iv = NULL;
	this->callback_ii = NULL;
}


void
LocalAction::Add(char *address, void (*callback)(int value))
{
	this->address = address;
	this->callback_vv = NULL;
	this->callback_vi = callback;
	this->callback_iv = NULL;
	this->callback_ii = NULL;
}


void
LocalAction::Add(char *address, int (*callback)(void))
{
	this->address = address;
	this->callback_vv = NULL;
	this->callback_vi = NULL;
	this->callback_iv = callback;
	this->callback_ii = NULL;
}


void
LocalAction::Add(char *address, int (*callback)(int value))
{
	this->address = address;
	this->callback_vv = NULL;
	this->callback_vi = NULL;
	this->callback_iv = NULL;
	this->callback_ii = callback;
}


void
LocalAction::Remove(void)
{
	this->address = NULL;
	this->callback_vv = NULL;
	this->callback_vi = NULL;
	this->callback_iv = NULL;
	this->callback_ii = NULL;
}


void
LocalAction::operator () (void)
{
	if (address == NULL) {
		client->ORState(STATUS_MALFORMED_COMMAND);
		return;
	}
	
	int arg, ret;
	
	if        (callback_vv) {
		callback_vv();
		client->WriteCommand(COMMAND_RETURN);
		
	} else if (callback_vi) {
		client->ReadInt(&arg);
		callback_vi(arg);
		client->WriteCommand(COMMAND_RETURN);
		
	} else if (callback_iv) {
		ret = callback_iv();
		client->WriteCommand(COMMAND_RETURN);
		client->WriteInt(&ret);
		
	} else if (callback_ii) {
		client->ReadInt(&arg);
		ret = callback_ii(arg);
		client->WriteCommand(COMMAND_RETURN);
		client->WriteInt(&ret);
	}
}


void
LocalAction::Register(void)
{
	if (address != NULL) {
		client->WriteCommand(COMMAND_ADD_ACTION);
		client->WriteActionID(&id);
		client->WriteString(address);
		
		if        (callback_vv) {
			client->WriteType(TYPE_VOID);
			client->WriteType(TYPE_VOID);
		} else if (callback_vi) {
			client->WriteType(TYPE_VOID);
			client->WriteType(TYPE_INT);
		} else if (callback_iv) {
			client->WriteType(TYPE_INT);
			client->WriteType(TYPE_VOID);
		} else if (callback_ii) {
			client->WriteType(TYPE_INT);
			client->WriteType(TYPE_INT);
		}
	}
}


void
LocalAction::Unregister(void)
{
	if (address != NULL) {
		client->WriteCommand(COMMAND_REMOVE_ACTION);
		client->WriteActionID(&id);
	}
}

}
