#include "WProgram.h"
#include "Comms.h"
#include "SHETSource.h"

using namespace SHETSource;


void
LocalAction::Setup(static char *address, void (*callback)(void))
{
	this->address = address;
	this->callback_vv = callback;
	this->callback_vi = NULL;
	this->callback_iv = NULL;
	this->callback_ii = NULL;
}


void
LocalAction::Setup(static char *address, void (*callback)(int value))
{
	this->address = address;
	this->callback_vv = NULL;
	this->callback_vi = callback;
	this->callback_iv = NULL;
	this->callback_ii = NULL;
}


void
LocalAction::Setup(static char *address, int (*callback)(void))
{
	this->address = address;
	this->callback_vv = NULL;
	this->callback_vi = NULL;
	this->callback_iv = callback;
	this->callback_ii = NULL;
}


void
LocalAction::Setup(static char *address, int (*callback)(int value))
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

