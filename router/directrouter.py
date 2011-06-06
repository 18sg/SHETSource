#!/usr/bin/python

from twisted.internet import reactor
from twisted.internet.serialport import SerialPort

from shet.client import ShetClient

from client import Client



if __name__ == "__main__":
	# Connect to SHET.
	shet = ShetClient()
	shet.install()
	
	# Connect a client to shet
	client = Client(shet)
	
	# ...and finally connect the client to the serial port.
	SerialPort(client, "/dev/arduino", reactor, baudrate=57600)
	
	reactor.run()
