#!/usr/bin/python


from fsm import FSM, state


class GatewayIO(FSM):
	"""
	A mixin for the Gateway object which provides reading and writing to and from
	the gateway's clients.
	"""
	
	def __init__(self):
		FSM.__init__(self)
	
	
	def write(self, client_address, data):
		"""
		Write some data to a client connected to the gateway. Each byte of data is
		sent individually and is expanded to three bytes:
		  1: The client_address is sent as a single byte, here the first 4 bits must
		     be 0s (so the client_address is limited to a maximum value of 2**4.
		  2: The first nibble of the byte is sent with a prefix of 0b1111
		  3: The second nibble of the byte  is sent with a prefix of 0b1011
		This is done to ensure that the gateway does not go out of sync when
		recieveing or sending data.
		"""
		
		for byte in data:
			print " "*40 + "Snd: %d: %s"%(client_address, repr(byte))
			encoded_data = (chr(client_address)
			                + chr(0b11110000 | (ord(byte) & 0x0F))
			                + chr(0b10110000 | ((ord(byte) & 0xF0) >> 4)))
			self.transport.writeSomeData(encoded_data)
	
	
	##############################################################################
	# Reader code/fsm                                                            #
	##############################################################################
	
	def dataReceived(self, data):
		"""
		Accept data recieved over the serial port. (Overridden from twisted protoocol.)
		"""
		self.process(data)
	
	
	@state
	def read_address(self, byte):
		"""First state of the read process: get the ID of the device."""
		
		int_byte = ord(byte)
		
		if int_byte & 0b11110000 != 0:
			# A sync-error has occurred: a non-address byte has been recieved.
			return self.read_address
		else:
			# The address was recieved
			self.state_client_address = int_byte
			return self.read_first_nibble
	
	
	@state
	def read_first_nibble(self, byte):
		"""Second state of the read process: get the first nibble of data."""
		
		int_byte = ord(byte)
		
		if int_byte & 0b11110000 != 0b11110000:
			# A sync-error has occurred: a non-first-data byte has been recieved.
			return self.read_address
		else:
			# The data was recieved
			self.state_first_nibble = int_byte & 0b00001111
			return self.read_second_nibble
	
	
	@state
	def read_second_nibble(self, byte):
		"""Third state of the read process: get the second nibble of data."""
		
		int_byte = ord(byte)
		
		if int_byte & 0b10110000 == 0b10110000:
			# The data was recieved
			self.state_second_nibble = int_byte & 0b00001111
			
			data = chr((self.state_second_nibble << 4) | self.state_first_nibble)
			
			self.data_recieved(self.state_client_address, data)
		
		return self.read_address
	
	
	"""Start the FSM on the address-reader."""
	initial_state = read_address

