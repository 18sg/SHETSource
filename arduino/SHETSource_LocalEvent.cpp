#include "WProgram.h"
#include "Comms.h"
#include "SHETSource.h"

using namespace SHETSource;


void
LocalEvent::Setup(static char *address)
{
	this->address = address;
}


void
LocalEvent::Remove(void)
{
	this->address = NULL;
}

