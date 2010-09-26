#!/usr/bin/python

from typefsm import *
from commands import *


class ClientSHETSource(object):
	
	
	def device_call_action(self, action_id, argument, argument_type):
		data  = Byte.encode(COMMAND_CALL_ACTION)
		data += Integer.encode(action_id)
		data += argument_type.encode(argument)
		self.write(data)
	
	
	def device_get_property(self, prop_id):
		data  = Byte.encode(COMMAND_GET_PROPERTY)
		data += Integer.encode(prop_id)
		self.write(data)
	
	
	def device_set_property(self, prop_id, value, contained_type):
		data  = Byte.encode(COMMAND_SET_PROPERTY)
		data += Integer.encode(prop_id)
		data += contained_type.encode(value)
		self.write(data)
	
	
	def device_reset(self):
		print "="*80
		data = Byte.encode(COMMAND_RESET);
		self.write(data)
