#include "WProgram.h"
#include "pins.h"
#include "comms.h"
#include "SHETSource.h"

using namespace SHETSource;


bool
Client::Read(uint8_t *buf, int len)
{
	bool success;
	success = comms->Read(buf, len);
	
	if (!success)
		ORState(STATUS_READ_FAIL);
	
	return success;
}


bool
Client::Write(uint8_t *buf, int len)
{
	bool success;
	success = comms->Write(buf, len);
	
	if (!success)
		ORState(STATUS_WRITE_FAIL);
	
	return success;
}


bool
Client::Write(char *str)
{
	bool success;
	success = comms->Write(str);
	
	if (!success)
		ORState(STATUS_WRITE_FAIL);
	
	return success;
}
