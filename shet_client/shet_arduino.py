# XXX: Massive refactoring needed: a quick hack-job

from command_nos import Commands

from fsm import FSM, state

from shet.client import ShetClient
from twisted.internet import reactor
from twisted.internet import defer

from twisted.internet.error import AlreadyCancelled
from twisted.internet.error import AlreadyCalled

import _types


# Number of inactive seconds to wait before sending a ping to a device
NO_READ_PING_TIMEOUT = 6

# Timeout for a ping command to return
PING_TIMEOUT = 1


class ArduinoShetClient(ShetClient, FSM):
	"""
	A SHET Client which forwards the actions, events and properties set up on the
	Arduino.
	"""
	
	def __init__(self, writer, *args, **kwargs):
		"""
		@param serial A gateway protocol object.
		"""
		self.writer = writer
		
		# Are we waiting for the device to send a reset
		self.awaiting_reset = True
		self.reset_sent = True
		
		# The address that all events etc. appear beneath.
		self.base_address = None
		
		self.ping_timer = None
		self.ping_timeout = None
		
		self.registered_actions = {}
		self.registered_events = {}
		self.registered_properties = {}
		
		# A queue of deferreds for requests sent to the arduino.
		self.deferred_returns = []
		
		ShetClient.__init__(self, *args, **kwargs)
		FSM.__init__(self)
	
	
	def reset_ping_timer(self):
		if self.ping_timeout is not None:
			try:
				self.ping_timeout.cancel()
			except AlreadyCancelled:
				pass
			except AlreadyCalled:
				pass
		
		if not self.awaiting_reset:
			if self.ping_timer is not None:
				try:
					self.ping_timer.reset(NO_READ_PING_TIMEOUT)
					return
				except AlreadyCancelled:
					pass
			
			self.ping_timer = reactor.callLater(NO_READ_PING_TIMEOUT, self.send_ping)
	
	
	def send_ping(self):
		self.ping_timer = None
		
		data = _types.Command().encode(Commands.COMMAND_PING)
		self.writer.write(data)
		
		d = defer.Deferred()
		d.return_type = _types.Int
		self.deferred_returns.append(d)
		
		self.ping_timeout = reactor.callLater(PING_TIMEOUT, self.timeout_device)
	
	
	"""
	Reset the connection to the SHET server clearning all events etc. and
	setting the base address for the client's events etc.
	"""
	def reset(self, base_address):
		# Set the new address
		self.base_address = base_address
		
		self.unregister_all()
		
		self.reset_ping_timer()
		
		self.reset_sent = False
		self.awaiting_reset = False
		print "!!!!Register!!!!", repr(base_address)
	
	
	def unregister_all(self):
		# Clear the registered events etc.
		self.registered_actions = {}
		self.registered_events = {}
		self.registered_properties = {}
		
		ShetClient.reset(self)
		
		for deferred in self.deferred_returns:
			deferred.errback(Exception("Arduino was reset."))
		
		self.deferred_returns = []
		
		# XXX: This should be replaced with something to automagically tell the SHET
		# Server to forget everything.
		if self.client is not None:
			self.client.transport.loseConnection()
	
	
	def send_call_action(self, action_id, arg, return_type):
		data = _types.Command().encode(Commands.COMMAND_CALL_ACTION)
		data += _types.Int().encode(action_id)
		data += return_type().encode(arg)
		self.writer.write(data)
	
	
	def send_get_property(self, property_id, contained_type):
		data = _types.Command().encode(Commands.COMMAND_GET_PROPERTY)
		data += _types.Int().encode(property_id)
		self.writer.write(data)
	
	
	def send_set_property(self, property_id, value, contained_type):
		data = _types.Command().encode(Commands.COMMAND_SET_PROPERTY)
		data += _types.Int().encode(property_id)
		data += contained_type().encode(value)
		self.writer.write(data)
	
	
	def reset_device(self):
		# Disconnect from SHET
		self.unregister_all()
		
		# Reset the device
		if not self.reset_sent:
			data = _types.Command().encode(Commands.COMMAND_RESET)
			self.writer.write(data)
			
			self.reset_sent = True
		
		self.awaiting_reset = True
		
		# Reset the FSM
		self.state = self.process_command
	
	
	def timeout_device(self):
		self.reset_device()
		self.reset_sent = False
	
	
	@state
	def process_command(self, command):
		command = ord(command)
		if command == Commands.COMMAND_RESET:
			self.reset((yield _types.StringNull))
			return
		elif self.awaiting_reset:
			self.reset_device()
			return
		
		
		
		if command == Commands.COMMAND_RETURN:
			d = self.deferred_returns.pop(0)
			d.callback((yield d.return_type) if hasattr(d, "return_type")
			                                    and d.return_type is not _types.Void
			                                 else None)
			return
			
		elif command == Commands.COMMAND_PING:
			return
			
			
		elif command == Commands.COMMAND_ADD_ACTION:
			action_id = (yield _types.Int)
			address = (yield _types.StringNull)
			return_type = (yield _types.TypeType)
			argument_type = (yield _types.TypeType)
			
			def action_called(arg = None):
				self.send_call_action(action_id, arg, return_type)
				
				d = defer.Deferred()
				d.return_type = return_type
				self.deferred_returns.append(d)
				return d
			
			action = self.add_action(self.base_address + address, action_called)
			action.argument_type = argument_type
			self.registered_actions[action_id] = action
			
			
		elif command == Commands.COMMAND_ADD_EVENT:
			event_id = (yield _types.Int)
			address = (yield _types.StringNull)
			
			event = self.add_event(self.base_address + address)
			self.registered_events[event_id] = event
			
		elif command == Commands.COMMAND_ADD_PROPERTY:
			property_id = (yield _types.Int)
			address = (yield _types.StringNull)
			contained_type = (yield _types.TypeType)
			
			def get_property():
				self.send_get_property(property_id, contained_type)
				
				d = defer.Deferred()
				d.return_type = contained_type
				self.deferred_returns.append(d)
				return d
			
			def set_property(value):
				self.send_set_property(property_id, value, contained_type)
			
			property = self.add_property(self.base_address + address,
			                             get_property, set_property)
			property.contained_type = contained_type
			self.registered_properties[property_id] = property
			
			
		elif command == Commands.COMMAND_REMOVE_ACTION:
			action_id = (yield _types.Int)
			self.remove_action(self.registered_actions[action_id])
			
		elif command == Commands.COMMAND_REMOVE_EVENT:
			event_id = (yield _types.Int)
			self.remove_event(self.registered_events[event_id])
			
		elif command == Commands.COMMAND_REMOVE_PROPERTY:
			property_id = (yield _types.Int)
			self.remove_property(self.registered_properties[property_id])
			
		elif command == Commands.COMMAND_RAISE_EVENT:
			event_id = (yield _types.Int)
			value_type = (yield _types.TypeType)
			value = (yield value_type if value_type is not _types.Void
			         else None)
			
			
			if value is not None:
				self.registered_events[event_id](value)
			else:
				self.registered_events[event_id]()
		else:
			self.reset_device()
	
	
	initial_state = process_command
	
	
	def process(self, *args, **kwargs):
		FSM.process(self, *args, **kwargs)
		self.reset_ping_timer()
