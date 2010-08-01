#include "WProgram.h"
#include "Comms.h"
#include "SHETSource.h"

using namespace SHETSource;


void
Local::Init(Client *client, id_t id)
{
	this->client  = client;
	this->id      = id;
	this->address = 0;
}


bool
Local::InUse(void)
{
	return this->address != NULL;
}

