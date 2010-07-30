from fsm import state


def serialize(f):
	def g(self, done_callback, *args, **kwargs):
		iter = f(self, *args, **kwargs)
		
		def callback(value):
			try:
				type = iter.send(value)
				return type(callback).start
			except StopIteration:
				return done_callback(value)
		
		return iter.next()(callback).start
	return g

class Type(object):
	
	def __init__(self, callback):
		self.callback = callback
	
	@state
	def start(self, d):
		pass
	
	@property
	def value(self):
		pass



class TypeType(Type):
	
	@state
	def start(self, d):
		print d
		intType = Int(lambda x: self.callback(types[ord(x)]))
		return intType.start(d)



class Void(Type):
	@state
	def start(self, d):
		return self.callback(None)(d)


class Int(Type):
	@state
	def start(self, d):
		return self.callback(d)



class StringNull(Type):
	def __init__(self, *args, **kwargs):
		Type.__init__(self, *args, **kwargs)
		self.data = []
	
	@state
	def start(self, d):
		if d == '\0':
			return self.callback(''.join(self.data))
		else:
			self.data.append(d)


types = {0: Void,
	1: Int,
	2: StringNull}
