#!/usr/bin/env python2
from twisted.internet import reactor
from twisted.internet.protocol import Factory

from shet.client import ShetClient
from client import Client


class ClientFactory(Factory):
	"""Create new ShetSource Clients connected to the given ShetClient."""
	
	def __init__(self, shet):
		self.shet = shet
	
	def buildProtocol(self, addr):
		return Client(self.shet)


def get_args():
	import argparse
	parser = argparse.ArgumentParser(description="SHETSource TCP router server.")
	parser.add_argument("--port", "-p", default=11236, type=int,
	                    help="The port to listen on. default=%(default)s")
	return parser.parse_args()

if __name__ == "__main__":
	args = get_args()
	
	# Connect to SHET.
	shet = ShetClient()
	shet.install()
	
	reactor.listenTCP(args.port, ClientFactory(shet))
	
	reactor.run()
