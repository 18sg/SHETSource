from shet.client import ShetClient
from twisted.internet import reactor
from twisted.internet import defer

from twisted.internet.protocol import Protocol
from twisted.internet.serialport import SerialPort


class ArduinoTest(Protocol):
	"""A protocol that will be passed data from the serial port.
	(It's just a protocol though - it could be connected to anything)
	"""
	def __init__(self, shet):
		#Protocol.__init__(self)
		self.shet = shet
		print self.transport
		print dir(self.transport)
	
	
	def dataReceived(self, data):
		"""Called by twisted with a string read from the serial port.
		"""
		for c in data:
			print "received: %s" % hex(ord(c))
			# Fire the shet event.
			self.shet.event(hex(ord(c)))



class TestShetClient(ShetClient):
	"""A shet client that reads from the serial port, and fires event '/test'
	whenever data is received.
	"""
	def __init__(self):
		ShetClient.__init__(self)
		# create an instance of the protocol, with a reference to this
		
		arduino = ArduinoTest(self)
		# connect it to the serial port
		SerialPort(arduino, "/dev/ttyUSB0", reactor)
		
		# standard shet stuff.
		self.event = self.add_event("test")
		
		self.action = self.add_action("act", self.on_act)
	
	
	def on_act(self, *args):
		print "act!"


def main():
	# Create the shet client and run it.
	TestShetClient().run("localhost")

if __name__ == "__main__":
	main()
