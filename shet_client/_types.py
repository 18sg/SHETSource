from fsm import FSM, state


class TypeReader(FSM):
	
	def __init__(self, *args, **kwargs):
		FSM.__init__(self, *args, **kwargs)



class CommandReader(Type):
	"""
	An FSM that reads a command ID and returns it.
	"""
	
	@state
	def start(self, d):
		return self.end_state_callback(ord(d))



class TypeTypeReader(Type):
	"""
	An FSM that reads a type ID and returns the appropriate FSM for reading that
	type.
	"""
	
	@state
	def start(self, d):
		specified_type = types[ord(d)]
		return self.end_state_callback(specified_type)



class VoidReader(Type):
	"""
	An FSM that does nothing but pass on the call to the next state.
	"""
	@state
	def start(self, *args, **kwargs):
		# The void reader does not accept any data and simply passes it on to the
		# next state.
		next_state = self.end_state_callback()
		return next_state(*args, **kwargs)


class IntReader(Type):
	"""
	An FSM that reads 16-bit signed integers
	"""
	
	def __init__(self, *args, **kwargs):
		Type.__init__(self, *args, **kwargs)
		
		# The endian-ness of the integers to be recieved
		self.little_endian = True
		# The number of bits in the integer
		self.size = 16
		
		# An internal store of the value as recieved so far.
		self.value = 0
		
		self.bits_read
	
	
	@state
	def start(self, byte):
		self.value = ord(byte) << (self.bits_read if self.little_endian
		                           else self.size - self.bits_read - 8)
		
		self.bits_read += 8
		
		if self.bits_read == self.size:
			# Convert unsigned number into signed number
			self.value = (((self.value + (1<<(self.size-1))) % (1<<self.size))
			              - (1<<(self.size-1)))
			
			return self.end_state_callback(self.value)
	
	
	initial_state = start



class StringNullReader(Type):
	"""
	A FSM that reads null-terminated strings.
	"""
	
	def __init__(self, *args, **kwargs):
		Type.__init__(self, *args, **kwargs)
		self.string = ""
	
	
	@state
	def start(self, byte):
		if byte == '\0':
			return self.end_state_callback(self.string)
		else:
			self.string += byte


types = {
	0: VoidReader,
	1: IntReader,
	2: StringNullReader
}
