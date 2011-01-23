#!/usr/bin/python


from twisted.internet import reactor

from error import NotConnected

from clientfsm        import ClientFSM
from clientshet       import ClientSHET
from clientshetsource import ClientSHETSource


class Client(ClientSHET, ClientFSM, ClientSHETSource):
	
	"""
	The number of secconds where no reads occur while the device is connected
	until a ping is sent to the device.
	"""
	INACTIVITY_TIMEOUT = 6
	
	
	def __init__(self, router, address):
		self.router  = router
		self.address = address
		
		ClientFSM.__init__(self)
		ClientSHET.__init__(self)
		ClientSHETSource.__init__(self)
		
		self.base_path = None
		self._connected = False
		self._deferred_returns = []
		
		self.connection_id = None
		
		self._properties = {}
		self._events     = {}
		self._actions    = {}
	
	
	def _get_connected(self):
		return self._connected
	
	def _set_connected(self, connected):
		assert(self._connected != connected)
		
		if not connected: self._on_disconnect()
		elif connected:   self._on_connect()
		
		self._connected = connected
	
	connected = property(fget = _get_connected, fset = _set_connected)
	
	
	def _on_connect(self):
		# Setup a timer for inactivity
		self._inactivity_timer = reactor.callLater(Client.INACTIVITY_TIMEOUT,
		                                           self._on_inactivity_timeout)
	
	def _on_disconnect(self):
		print ("DISCONNECTED!"*80)[:80]
		self._clear_shet()
		
		self.base_path = None
		
		self.connection_id = None
		
		# Clear all waiting returns
		for deferred_return in self._deferred_returns:
			deferred_return.errback(NotConnected())
		self._deferred_returns = []
		
		# Cancel the inactivity timer
		if self._inactivity_timer.active():
			self._inactivity_timer.cancel()
		
		self.state = self.process_command
	
	
	def write(self, data):
		"""Write some data to a client via the gateway."""
		self.router.gateway.write(self.address, data)
	
	
	def data_recieved(self, data):
		"""Called when this client recieves some data from the gateway."""
		self.reset_inactivity_timer()
		try:
			self.process(data)
		except Exception, e:
			self._on_malformed_command()
			print e
	
	
	def reset_inactivity_timer(self):
		"""Reset the timer which sends a inactivity after a period of inactivity."""
		if self.connected:
			# Reset the inactivity timer if it is active
			if self._inactivity_timer.active():
				self._inactivity_timer.reset(Client.INACTIVITY_TIMEOUT)
	
	
	def _on_inactivity_timeout(self):
		print ("TIMEOUT!"*80)[:80]
		self.connected = False
	
	
	def _on_malformed_command(self, data = None):
		print ("MALFORMED!"*80)[:80]
		if self.connected:
			self.connected = False
		self.device_reset()
	
	
	def path(self, address):
		"""Convert an address into a full SHET path for this device."""
		if not address.startswith("/"):
			return "/".join((self.base_path, address))
		else:
			return address
