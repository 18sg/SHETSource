#!/usr/bin/env python2

import sys

from twisted.internet import reactor
from twisted.internet.serialport import SerialPort

from shet.client import ShetClient

from gateway import Gateway


def get_args():
	import argparse
	parser = argparse.ArgumentParser(description="SHETSource router.")
	parser.add_argument("device", default="/dev/arduino", nargs="?",
	                    help="The /dev device of the arduino. default=/dev/arduino")
	return parser.parse_args()


class StoppingGateway(Gateway):
	"""A gateway that stops the reactor when the connection is lost."""
	
	def  connectionLost(self, reason):
		Gateway.connectionLost(self, reason)
		reactor.stop()


if __name__ == "__main__":
	args = get_args()
	
	# Connect to SHET.
	shet = ShetClient()
	shet.install()
	
	# Connect a gateway to shet
	gateway = StoppingGateway(shet)
	
	# ...and finally connect the gateway to the serial port.
	SerialPort(gateway, args.device, reactor, baudrate=115200)
	
	reactor.run()
