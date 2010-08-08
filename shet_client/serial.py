from twisted.internet.serialport import SerialPort

from twisted.internet.protocol import Protocol

from fsm import FSM, state
import commands

class SerialProtocol(Protocol, FSM):
	
	@state
	def start(self, d):
		self.hardware_address = d
		return self.command
	
	
	@state
	def command(self, command):
		self.command_no = command
		me = commands.MakeEvent(self, None)
		
		return me.start
	
	def test(self, id, name, type):
		print id, name, type
	
	
	initial_state = start
	
	
	def __init__(self, address, port=11357):
		FSM.__init__(self)
		self.clients = []
	
	
	def dataReceived(self, data):
		self.process(data)


if __name__ == "__main__":
	s = SerialProtocol("localhost")
	s.process("\0\0\5foo\0\1")
