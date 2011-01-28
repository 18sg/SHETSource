#!/usr/bin/python


from twisted.internet import reactor

from shet.client import ShetClient

from gateway import Gateway


class Router(object):
	
	def __init__(self, shet_server   = "localhost",
	                   shet_port     = 11235,
	                   gateway_port  = "/dev/ttyUSB0",
	                   gateway_speed = 115200):
		
		self.shet = ShetClient()
		self.shet.install(shet_server, shet_port)
		
		self.gateway = Gateway(self, gateway_port, gateway_speed)
	
	
	def path(self, address):
		"""Convert an address into a full path."""
		return "/%s"%address
	
	
	def run(self):
		reactor.run()


if __name__ == "__main__":
	router = Router()
	router.run()
