from fsm import state
import _types


class CommandRecv(object):
	
	def __init__(self, serial, shet):
		self.serial = serial
		self.shet = shet
	
	@state
	def start(self, *args, **kwargs):
		# The real state is the first state of the get_params 'sub
		# FSM' which when finished will continue executing from
		# the start state of the serial state machine. Because
		# this function is essentially an alias, just "pass on"
		# the call to the state.
		return self.get_params(self.serial.start)(*args, **kwargs)
		

class CommandSend(object):
	
	def send(self):
		# call send_data here
		pass
	
	def send_data(self, data):
		

class Reset(CommandRecv):
	
	@state
	@_types.sub_fsm
	def get_params(self):
		self.id = (yield _types.StringNull)



class MakeEvent(CommandRecv):
	
	@state
	@_types.sub_fsm
	def get_params(self):
		self.id = (yield _types.Int)
		
		self.name = (yield _types.StringNull)
		
		self.type = (yield _types.TypeType)
		print
		self.serial.test(self.id, self.name, self.type)
		#self.shet.add_event(self.name, self.type)
