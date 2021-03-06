#include <Arduino.h>
#include "pins.h"
#include "comms.h"
#include "SHETSource.h"

namespace SHETSource {

void
Client::OnRcvReset(void)
{
	ORState(STATUS_RESETTING);
}


void
Client::OnRcvPing(void)
{
	int state = GetState();
	WriteCommand(COMMAND_RETURN);
	WriteInt(&state);
}


void
Client::OnRcvCallAction(void)
{
	action_id_t action_id;
	if (!ReadActionID(&action_id)) return;
	
	if (action_id >= NUM_ACTIONS) {
		ORState(STATUS_MALFORMED_COMMAND);
		return;
	}
	
	actions[action_id]();
}


void
Client::OnRcvSetProperty(void)
{
	property_id_t property_id;
	if (!ReadPropertyID(&property_id)) return;
	
	if (property_id >= NUM_PROPERTIES) {
		ORState(STATUS_MALFORMED_COMMAND);
		return;
	}
	
	properties[property_id].Set();
}


void
Client::OnRcvGetProperty(void)
{
	property_id_t property_id;
	if (!ReadPropertyID(&property_id)) return;
	
	if (property_id >= NUM_PROPERTIES) {
		ORState(STATUS_MALFORMED_COMMAND);
		return;
	}
	
	properties[property_id].Get();
}

}
