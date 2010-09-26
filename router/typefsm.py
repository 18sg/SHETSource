#!/usr/bin/python


from fsm import FSM, state


class TypeFSM(FSM):
	
	def __init__(self, *args, **kwargs):
		FSM.__init__(self, *args, **kwargs)


class Byte(TypeFSM):
	
	@state
	def start(self, byte):
		return self.end_state_callback(ord(byte))
	initial_state = start
	
	@staticmethod
	def encode(value):
		return chr(value)



class Void(TypeFSM):
	
	@state
	def start(self, *args, **kwargs):
		# The void reader does not accept any data and simply passes it on to the
		# next state.
		next_state = self.end_state_callback()
		return next_state(*args, **kwargs)
	
	@staticmethod
	def encode(value):
		return ""



class Integer(TypeFSM):
	
	# The endian-ness of the integers to be recieved
	little_endian = True
	# The number of bits in the integer
	size = 16
	
	def __init__(self, *args, **kwargs):
		TypeFSM.__init__(self, *args, **kwargs)
		
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
	
	
	@staticmethod
	def encode(value):
		output = ""
		
		rng = [0, Integer.size, 8] if Integer.little_endian else [Integer.size - 8, -1, -8]
		for offset in range(*rng):
			output += chr((value >> offset) & 255)
		
		return output



class String(TypeFSM):
	
	def __init__(self, *args, **kwargs):
		TypeFSM.__init__(self, *args, **kwargs)
		self.string = ""
	
	
	@state
	def start(self, byte):
		if byte == '\0':
			return self.end_state_callback(self.string)
		else:
			self.string += byte
	
	initial_state = start
	
	
	@staticmethod
	def encode(value):
		return value + "\x00"



class Type(TypeFSM):
	"""
	An FSM that reads a type ID and returns the appropriate FSM for reading that
	type.
	"""
	
	types = {
		0: Void,
		1: Integer,
		2: String
	}
	
	@state
	def start(self, d):
		specified_type = Type.types[ord(d)]
		return self.end_state_callback(specified_type)
	
	initial_state = start
	
	
	@staticmethod
	def encode(value):
		for id, type in Type.types.iteritems():
			if type is value:
				return chr(id)
		
		raise IndexError("Unknown type being encoded!")
