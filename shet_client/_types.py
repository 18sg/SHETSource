from fsm import FSM, state


class Type(FSM):
	
	def __init__(self, *args, **kwargs):
		FSM.__init__(self, *args, **kwargs)



class Command(Type):
	"""
	An FSM that reads a command ID and returns it.
	"""
	
	@state
	def start(self, d):
		return self.end_state_callback(ord(d))
	
	
	initial_state = start
	
	
	def encode(self, value):
		if type(value) is int:
			return chr(value)
		else:
			return chr



class TypeType(Type):
	"""
	An FSM that reads a type ID and returns the appropriate FSM for reading that
	type.
	"""
	
	@state
	def start(self, d):
		specified_type = types[ord(d)]
		return self.end_state_callback(specified_type)
	
	initial_state = start
	
	
	def encode(self, value):
		for id, type in types.iteritems():
			if type is value:
				return chr(id)
		
		raise IndexError("Unknown type being encoded!")



class Void(Type):
	"""
	An FSM that does nothing but pass on the call to the next state.
	"""
	@state
	def start(self, *args, **kwargs):
		# The void reader does not accept any data and simply passes it on to the
		# next state.
		next_state = self.end_state_callback()
		return next_state(*args, **kwargs)
	
	
	def encode(self, value):
		return ""


class Int(Type):
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
		
		self.bits_read = 0
	
	
	@state
	def start(self, byte):
		self.value += ord(byte) << (self.bits_read if self.little_endian
		                            else self.size - self.bits_read - 8)
		
		self.bits_read += 8
		
		if self.bits_read == self.size:
			# Convert unsigned number into signed number
			self.value = (((self.value + (1<<(self.size-1))) % (1<<self.size))
			              - (1<<(self.size-1)))
			
			return self.end_state_callback(self.value)
	
	
	initial_state = start
	
	
	def encode(self, value):
		output = ""
		
		rng = [0, self.size, 8] if self.little_endian else [self.size - 8, -1, -8]
		for offset in range(*rng):
			output += chr((value >> offset) & 255)
		
		return output



class StringNull(Type):
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
	
	initial_state = start
	
	
	def encode(self, value):
		return value + "\x00"


types = {
	0: Void,
	1: Int,
	2: StringNull
}
