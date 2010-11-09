#!/usr/bin/python

from twisted.internet import reactor

from time import time

from typefsm import *
from commands import *

RESET_COOLOFF_DELAY = 10


class ClientSHETSource(object):
	
	def __init__(self):
		self.last_reset = 0
		self.delayed_reset = None
	
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
	
	
	def _send_device_reset(self):
		print "="*80
		data = Byte.encode(COMMAND_RESET);
		self.write(data)
		self.delayed_reset = None
		self.last_reset = time()
	
	
	def cancel_reset(self):
		if self.delayed_reset is not None:
			if self.delayed_reset.active():
				self.delayed_reset.cancel()
			self.delayed_reset = None
	
	
	def device_reset(self):
		if self.last_reset + RESET_COOLOFF_DELAY < time():
			self._send_device_reset()
		elif self.delayed_reset is not None:
			self.delayed_reset = reactor.callLater(
			                       self.last_reset + RESET_COOLOFF_DELAY,
			                       self._send_device_reset)
