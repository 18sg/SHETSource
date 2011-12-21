#!/usr/bin/python

import sys

from twisted.internet import reactor
from twisted.internet.serialport import SerialPort

from shet.client import ShetClient

from gateway import Gateway


if __name__ == "__main__":
	# Connect to SHET.
	shet = ShetClient()
	shet.install()
	
	# Connect a gateway to shet
	gateway = Gateway(shet)
	
	# ...and finally connect the gateway to the serial port.
	port = sys.argv[1] if len(sys.argv) >= 2 else "/dev/ttyUSB0"
	SerialPort(gateway, port, reactor, baudrate=115200)
	
	reactor.run()
