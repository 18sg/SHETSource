from command_nos import Commands

from shet.client import ShetClient
from twisted.internet import reactor
from twisted.internet import defer

class ArduinoShetClient(ShetClient, FSM):
	"""
	A SHET Client which forwards the actions, events and properties set up on the
	Arduino.
	"""
	
	def __init__(self, gateway, *args, **kwargs):
		"""
		@param serial A gateway protocol object.
		"""
		self.gateway = gateway
		
		# The address that all events etc. appear beneath.
		self.base_address = None
		
		self.registered_actions = {}
		self.registered_events = {}
		self.registered_properties = {}
		
		# A queue of deferreds for requests sent to the arduino.
		self.deferred_returns = []
		
		ShetClient.__init__(self, *args, **kwargs)
		FSM.__init__(self)
	
	
	"""
	Reset the connection to the SHET server clearning all events etc. and
	setting the base address for the client's events etc.
	"""
	def reset(self, base_address):
		# Set the new address
		self.base_address = base_address
		
		# Clear the registered events etc.
		self.registered_actions = {}
		self.registered_events = {}
		self.registered_properties = {}
		
		for deferred in self.deferred_returns:
			deferred.errback(Exception("Arduino was reset."))
		
		self.deferred_returns = []
		
		# Request to the gateway for this client be reset
		self.gateway.reset_client(self)
	
	
	def send_call_action(self, action_id, arg, return_type):
		pass
	
	
	def send_get_property(self, property_id, contained_type):
		pass
	
	
	def send_set_property(self, property_id, value, contained_type):
		pass
	
	
	@state
	def process_command(self):
		command = (yield _types.CommandReader)
		
		if command == Commands.COMMAND_RESET:
			self.reset(yield _types.StringNullReader)
			return
		
		elif command == Commands.COMMAND_RETURN:
			d = self.deferred_returns.pop(0)
			d.callback(yield d.return_type if d.return_type is not _types.VoidReader
			           else None)
			return
			
		elif command == Commands.COMMAND_ADD_ACTION:
			action_id = (yield _types.IntReader)
			address = (yield _types.StringNullReader)
			return_type = (yield _types.TypeTypeReader)
			argument_type = (yield _types.TypeTypeReader)
			
			def action_called(arg = None):
				self.send_call_action(action_id, arg, return_type)
				
				d = defer.Deferred()
				self.deferred_returns.append(d)
				return d
			
			action = self.add_action(self.base_address + address, action_called)
			action.argument_type = argument_type
			self.registered_actions[action_id] = action
			
		elif command == Commands.COMMAND_ADD_EVENT:
			event_id = (yield _types.IntReader)
			address = (yield _types.StringNullReader)
			
			event = self.add_event(self.base_address + address)
			self.registered_events[event_id] = event
			
		elif command == Commands.COMMAND_ADD_PROPERTY:
			property_id = (yield _types.IntReader)
			address = (yield _types.StringNullReader)
			contained_type = (yield _types.TypeTypeReader)
			
			def get_property():
				self.send_get_property(property_id, contained_type)
				
				d = defer.Deferred()
				self.deferred_returns.append(d)
				return d
			
			def set_property(value):
				self.send_set_property(property_id, value, contained_type)
			
			property = self.add_property(self.base_address + address,
			                             get_property, set_property)
			property.contained_type = contained_type
			self.registered_properties[property_id] = property
			
		elif command == Commands.COMMAND_REMOVE_ACTION:
			action_id = (yield _types.IntReader)
			self.remove_action(self.registered_actions[action_id])
			
		elif command == Commands.COMMAND_REMOVE_EVENT:
			event_id = (yield _types.IntReader)
			self.remove_event(self.registered_events[event_id])
			
		elif command == Commands.COMMAND_REMOVE_PROPERTY:
			property_id = (yield _types.IntReader)
			self.remove_property(self.registered_properties[property_id])
			
		elif command == Commands.COMMAND_RAISE_EVENT:
			event_id = (yield _types.IntReader)
			value_type = (yield _types.TypeTypeReader)
			value = (yield value_type if value_type is not _types.VoidReader
			         else None)
			
			if value is not None:
				self.registered_events[event_id](value)
			else:
				self.registered_events[event_id]()
	
	
	initial_state = process_command
