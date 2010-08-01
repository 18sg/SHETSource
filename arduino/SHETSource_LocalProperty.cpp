#include "WProgram.h"
#include "Comms.h"
#include "SHETSource.h"

using namespace SHETSource;


void
LocalProperty::Setup(static char *address,
                     void (*set_callback)(int value),
                     int  (*get_callback)(void))
{
	this->address = address;
	this->get_callback = get_callback;
	this->set_callback = set_callback;
	this->var = NULL;
}


void
LocalProperty::Setup(static char *address, int *var)
{
	this->address = address;
	this->get_callback = NULL;
	this->set_callback = NULL;
	this->var = var;
}


void
LocalProperty::Setup(static char *address, int *var)
{
	this->address = NULL;
	this->get_callback = NULL;
	this->set_callback = NULL;
	this->var = NULL;
}

