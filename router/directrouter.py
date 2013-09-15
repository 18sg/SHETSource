#!/usr/bin/env python2

from twisted.internet import reactor
from twisted.internet.serialport import SerialPort

from shet.client import ShetClient

from client import Client


def get_args():
	import argparse
	parser = argparse.ArgumentParser(description="SHETSource direct router.")
	parser.add_argument("device", default="/dev/arduino", nargs="?",
	                    help="The /dev device of the arduino. default=/dev/arduino")
	return parser.parse_args()


class StoppingClient(Client):
	
	def  connectionLost(self, reason):
		Client.connectionLost(self, reason)
		reactor.stop()


if __name__ == "__main__":
	args = get_args()
	
	# Connect to SHET.
	shet = ShetClient()
	shet.install()
	
	# Connect a client to shet
	client = StoppingClient(shet)
	
	# ...and finally connect the client to the serial port.
	SerialPort(client, args.device, reactor, baudrate=57600)
	
	reactor.run()
