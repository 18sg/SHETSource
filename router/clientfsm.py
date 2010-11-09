#!/usr/bin/python

from twisted.internet.defer import Deferred

from fsm import FSM, state

from commands import *

from typefsm import *


class ClientFSM(FSM):
	
	def __init__(self):
		FSM.__init__(self)
	
	
	@state
	def process_command(self, byte):
		"""
		Process a SHETSource command sent from an arduino.
		"""
		
		command = ord(byte)
		
		if not self.connected and command != COMMAND_RESET:
			self.device_reset()
		elif command == COMMAND_RESET:           return self.process_reset(byte)
		elif command == COMMAND_RETURN:          return self.process_return(byte)
		elif command == COMMAND_PING:            return self.process_ping(byte)
		elif command == COMMAND_ADD_ACTION:      return self.process_add_action(byte)
		elif command == COMMAND_ADD_EVENT:       return self.process_add_event(byte)
		elif command == COMMAND_ADD_PROPERTY:    return self.process_add_property(byte)
		elif command == COMMAND_REMOVE_ACTION:   return self.process_remove_action(byte)
		elif command == COMMAND_REMOVE_EVENT:    return self.process_remove_event(byte)
		elif command == COMMAND_REMOVE_PROPERTY: return self.process_remove_property(byte)
		elif command == COMMAND_RAISE_EVENT:     return self.process_raise_event(byte)
		else:
			self._on_malformed_command(byte)
			return
	
	
	initial_state = process_command
	
	
	@state
	def process_reset(self, byte):
		print ("RESET_RECIEVED!"*80)[:80]
		if self.connected:
			self.connected = False
		
		# Absorb the leading reset bytes
		while True:
			byte = (yield Byte)
			if byte == COMMAND_RESET:                continue
			elif byte == COMMAND_RESET ^ 0b11111111: break
			else:
				self._on_malformed_command(byte)
				return
		
		self.connection_id = (yield Integer)
		self.base_path     = self.router.path((yield String))
		self.connected     = True
		print ("CONNECTED!"*80)[:80]
	
	
	@state
	def process_return(self, byte):
		# Get the next queued return value requested
		deferred_return = self._deferred_returns.pop(0)
		
		# If there is an associated value, fetch it and raise the callback
		if deferred_return.return_type is not Void:
			deferred_return.callback((yield deferred_return.return_type))
		else:
			deferred_return.callback(None)
			return
	
	
	@state
	def process_ping(self, byte):
		connection_id = (yield Integer)
		if self.connection_id != connection_id:
			print ("WRONG_ID***"*80)[:80]
			self._on_malformed_command()
	
	
	@state
	def process_add_property(self, byte):
		prop_id        = (yield Integer)
		address        = (yield String)
		contained_type = (yield Type)
		
		def on_get_property():
			"""A callback which will forward a 'get' from SHET to the device."""
			self.device_get_property(prop_id)
			
			deferred_return = Deferred()
			
			# Inject the return type so that the return-handler knows how to read it
			deferred_return.return_type = contained_type
			
			self._deferred_returns.append(deferred_return)
			
			# Return a deferred for the return value.
			return deferred_return
		
		def on_set_property(value):
			"""A callback which will forward a 'set' from SHET to the device."""
			self.device_set_property(prop_id, value, contained_type)
		
		shet_prop = self.add_property(prop_id, self.path(address),
		                              on_get_property, on_set_property)
		
		# Inject the type
		shet_prop.contained_type = contained_type
	
	
	@state
	def process_add_action(self, byte):
		action_id     = (yield Integer)
		address       = (yield String)
		return_type   = (yield Type)
		argument_type = (yield Type)
		
		def on_action(argument = None):
			"""A callback which will forward a call from SHET to the device."""
			self.device_call_action(action_id, argument, argument_type)
			
			deferred_return = Deferred()
			
			# Inject the return type so that the return-handler knows how to read it
			deferred_return.return_type = return_type
			
			self._deferred_returns.append(deferred_return)
			
			# Return a deferred for the return value.
			return deferred_return
		
		shet_action = self.add_action(action_id, self.path(address), on_action)
		
		# Inject the argument type to send
		shet_action.argument_type = argument_type
	
	
	@state
	def process_add_event(self, byte):
		event_id = (yield Integer)
		address  = (yield String)
		
		shet_event = self.add_event(event_id, self.path(address))
	
	
	@state
	def process_remove_property(self, byte):
		prop_id = (yield Integer)
		self.remove_property(prop_id)
	
	
	@state
	def process_remove_action(self, byte):
		action_id = (yield Integer)
		self.remove_action(action_id)
	
	
	@state
	def process_remove_event(self, byte):
		event_id = (yield Integer)
		self.remove_event(event_id)
	
	
	@state
	def process_raise_event(self, byte):
		event_id   = (yield Integer)
		value_type = (yield Type)
		
		if value_type is not Void:
			self._events[event_id]((yield value_type))
		else:
			self._events[event_id]()

