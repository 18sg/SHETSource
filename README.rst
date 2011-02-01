.. image::https://github.com/18sg/SHETSource/raw/master/doc/logo.png
	:alt The SHETSource Logo

SHETSource
==========

Rationale
---------

SHETSource is a platform for attaching Arduinos to SHET. The system consists of:

* A C++ library for creating SHET Actions, Events and Properties directly on an
  Arduinos.
* An Arduino program for routing data from several connected Arduinos and a
  computer.
* A SHET client that translates between the simplified protocol used by the
  Arduino SHETSource library and SHET.

The aim of SHETSource is not to recreate the full functionality of SHET on the
Arduino but rather to allow Arduino-based clients to provide simple functions on
a SHET system which can be used by other SHET clients. The system allows simple
but effective ways to add 'real-world' I/O to SHET making SHET-based
home-automation practical.

About the Library
-----------------

SHETSource does not allow the Arduinos to listen, call or get/set events,
actions or properties on the SHET network. Clients are intended to be thin,
mainly IO-interface style systems. More complex things should be handled by
clients running using the regular SHET libraries where the whole functionality
of SHET is available.

Library is otherwise intended to be as similar to the regular SHET library as is
reasonable on such a platform.


Requirements
------------

SHETSource needs a central Arduino to act as a 'Gateway' for connecting all
other Arduinos to the computer. Each client Arduino is connected via two data
lines to the gateway (but this may be converted to a bus at a later date).


Architecture
------------

.. image::https://github.com/18sg/SHETSource/raw/master/doc/architecture.png
	:alt Architecture diagram showing several Arduinos connected to one Arduino and then to SHET via USB.

The SHETSource architecture consists of several Arduinos ('clients') which
connect to one central Arduino via a pair of data lines. The central ('gateway')
Arduino forwards data to and from the computer via USB. The Arduinos use a very
simple byte-oriented version of the SHET protocol which is interpreted by the
SHET client which then sends the corresponding SHET commands to the SHET server.

As each individual client requires two data pins on the gateway and its own
unique set of wires this system has some potential practical scale limitations.
A common bus connection is one of a list of long-term feature goals.

In the installation of SHETSource in the developer's house, the four spare wires
in 100mb Ethernet were used to connect +12v, 0v and two data lines from each
Arduino to the comms cupboard. This set up allowed us to make use of the
existing cabling in the house to connect up all the Arduinos to one machine.
