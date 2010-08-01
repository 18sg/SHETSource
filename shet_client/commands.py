from fsm import state
import _types


class CommandRecv(object):
	
	def __init__(self, serial, shet):
		self.serial = serial
		self.shet = shet
	
	@state
	def start(self, d):
		pass



# class MakeEvent(CommandRecv):
# 	
# 	@state
# 	def start(self, d):
# 		return _types.Int(self.read_id).start(d)
# 	
# 	def read_id(self, id):
# 		self.id = id
# 		return _types.StringNull(self.read_name).start
# 	
# 	def read_name(self, name):
# 		self.name = name
# 		return _types.TypeType(self.read_type)
# 	
# 	def read_type(self, type):
# 		self.type = type
# 		#self.shet.add_event(self.name, self.type)
# 		return self.serial.start


class MakeEvent(CommandRecv):
	
	@state
	def start(self, d):
		return self.get_params(self.serial.start)(d)
		
	@state
	@_types.serialize
	def get_params(self):
		self.id = (yield _types.Int)
		
		self.name = (yield _types.StringNull)
		
		self.type = (yield _types.TypeType)
		print
		self.serial.test(self.id, self.name, self.type)
		#self.shet.add_event(self.name, self.type)
