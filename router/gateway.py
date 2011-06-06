#!/usr/bin/python


from twisted.internet.protocol   import Protocol

from gatewayio import GatewayIO

from client import Client


class Gateway(Protocol, GatewayIO):

	def __init__(self, shet):
		self.shet = shet
		GatewayIO.__init__(self)
		
		self.clients = {}
	
	
	def data_recieved(self, client_address, data):
		"""Called when a byte of data is received from a client."""
		
		print "Rcv: %d: %s"%(client_address, repr(data))
		
		if client_address not in self.clients:
			client = Client(self.shet)
			self.clients[client_address] = ClientConnector(self, client, client_address)
		
		self.clients[client_address].dataReceived(data)
	
	
	def dataReceived(self, data): GatewayIO.dataReceived(self, data)
	def write(self, client_address, data): GatewayIO.write(self, client_address, data)


class ClientConnector(object):
	"""This class connects the gateway to a Client, acting as it's transport.
	This is here so that the clients don't have to know about their address.
	"""
	
	def __init__(self, gateway, protocol, address):
		self.gateway = gateway
		self.protocol = protocol
		self.address = address
		
		self.protocol.makeConnection(self)
	
	
	def write(self, data):
		"""Called by the protocol (Client) to write data to the device via the
		gateway.
		"""
		self.gateway.write(self.address, data)
	
	
	def dataReceived(self, data):
		"""Called by the gateway upon recieving data for the Client."""
		self.protocol.dataReceived(data)
