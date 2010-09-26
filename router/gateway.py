#!/usr/bin/python


from twisted.internet.serialport import SerialPort
from twisted.internet.protocol   import Protocol
from twisted.internet            import reactor

from gatewayio import GatewayIO

from client import Client


class Gateway(SerialPort, Protocol, GatewayIO):

	def __init__(self, router, serial_port, serial_speed):
		self.router      = router
		self.serial_port = serial_port
		
		GatewayIO.__init__(self)
		SerialPort.__init__(self, self, self.serial_port, reactor, serial_speed)
		
		self.clients = {}
	
	
	
	def data_recieved(self, client_address, data):
		"""Called when a byte of data is received from a client."""
		
		print "Rcv: %d: %s"%(client_address, repr(data))
		
		if client_address not in self.clients:
			self.clients[client_address] = Client(self.router, client_address)
		
		self.clients[client_address].data_recieved(data)
	
	
	def dataReceived(self, data): GatewayIO.dataReceived(self, data)
	def write(self, client_address, data): GatewayIO.write(self, client_address, data)
