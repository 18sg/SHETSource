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
		print " "*40 + "Write:", self.address, repr(data)
		for byte in data:
			encoded_data = (chr(0b11110000 | (ord(byte) & 0x0F))
			                + chr(0b10110000 | ((ord(byte) & 0xF0) >> 4)))
			self.transport.writeSomeData(chr(self.address) + encoded_data)



class SerialProtocol(Protocol, FSM):
	
	def __init__(self, address, port=11235):
		FSM.__init__(self)
		self.address = address
		self.port = port
		
		self.clients = {}
	
	
	def makeConnection(self, *args, **kwargs):
		Protocol.makeConnection(self, *args, **kwargs)
		
		self.sync_gateway()
	
	
	def sync_gateway(self):
		pass#self.transport.writeSomeData("\xFF\xFF")
	
	
	@state
	def get_address(self, d):
		data = ord(d)
		
		# Invalid data sent
		if data & 0b10000000 != 0:
			print "Sync error (on ID)"
			return self.get_address
		else:
			self.client_address = data
		
		if self.client_address not in self.clients:
			client = ArduinoShetClient(Writer(self.transport, self.client_address))
			client.install(self.address, self.port)
			
			self.clients[self.client_address] = client
		
		self.data = 0
		
		return self.get_data1
	
	
	@state
	def get_data1(self, d):
		data = ord(d)
		
		if (data & 0b11110000) != 0b11110000:
			print "Sync error (on d1)"
			return self.get_address
		else:
			self.data = data & 0x0F
			return self.get_data2
	
	
	@state
	def get_data2(self, d):
		data = ord(d)
		
		if (data & 0b11110000) != 0b10110000:
			print "Sync error (on d2)"
			return self.get_address
		else:
			self.data |= (data & 0x0F) << 4
		
		print self.clients[self.client_address].awaiting_reset, self.client_address, repr(chr(self.data))
		self.clients[self.client_address].process(chr(self.data))
		return self.get_address
	
	
	initial_state = get_address
	
	
	def dataReceived(self, data):
		self.process(data)


if __name__ == "__main__":
	p = SerialProtocol("localhost")
	transport = SerialPort(p, "/dev/ttyUSB0", reactor)
	reactor.run()
