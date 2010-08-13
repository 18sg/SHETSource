from twisted.internet.serialport import SerialPort
from twisted.internet.protocol import Protocol
from twisted.internet import reactor

from shet_arduino import ArduinoShetClient

from fsm import FSM, state

import _types


class Writer(object):
	
	def __init__(self, transport, address):
		self.transport = transport
		self.address = address
	
	
	def write(self, data):
		for byte in data:
			self.transport.writeSomeData(chr(self.address) + byte)



class SerialProtocol(Protocol, FSM):
	
	def __init__(self, address, port=11235):
		FSM.__init__(self)
		self.address = address
		self.port = port
		
		self.clients = {}
		
	
	@state
	def get_address(self, d):
		self.client_address = ord(d)
		
		if self.client_address not in self.clients:
			client = ArduinoShetClient(Writer(self.transport,
			                                  self.client_address))
			client.install(self.address, self.port)
			
			self.clients[self.client_address] = client
		
		return self.get_data
	
	
	@state
	def get_data(self, d):
		self.clients[self.client_address].process(d)
		return self.get_address
	
	
	initial_state = get_address
	
	
	def dataReceived(self, data):
		self.process(data)


if __name__ == "__main__":
	p = SerialProtocol("localhost")
	transport = SerialPort(p, "/dev/ttyUSB0", reactor)
	reactor.run()
