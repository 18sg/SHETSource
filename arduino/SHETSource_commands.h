#ifndef SHET_SOURCE_COMMANDS_H
#define SHET_SOURCE_COMMANDS_H

typedef enum _command_t {
	/**
	 * Slave -> Master:
	 *   This device has been connected for the first time or is reconnecting
	 *   after some error (e.g. communication error). If received then the device
	 *   will proceed to register all events.
	 *
	 *   Arguments:
	 *     \0    Relative address for this device (e.g. a name)
	 *
	 * Master -> Slave:
	 *   The master does not recognise this device (i.e. it has rejected it after
	 *   a communication error) yet has received a command from it other than a
	 *   reset. When received the slave should immediately perform a reset.
	 */
	COMMAND_RESET           = 0x01,
	
	
	/**
	 * Master -> Slave:
	 *   Sent on occasion to a slave to check to see if it is still
	 *   connected/responding. The clients response should be 8-bits which equal 0
	 *   if the device is in a normal state and otherwise if not.
	 *
	 *   If a slave doesn't received a ping or other communication for a certain
	 *   amount of time it assumes it has been disconnected.
	 *
	 *   Responses:
	 *     byte  A bit-field, 0 if device is working normally, otherwise if not.
	 */
	COMMAND_PING            = 0x02,
	
	
	/**
	 * Slave -> Master:
	 *   Sent to register an action with the server.
	 *
	 *   Arguments:
	 *     int   ID to use to reffer to this action from here on.
	 *     \0    Relative address of this action.
	 *     type  The type of argument required by the action
	 *     type  The type of return value from the action being executed.
	 */
	COMMAND_ADD_ACTION      = 0x03,
	
	
	/**
	 * Slave -> Master:
	 *   Sent to register an event with the server.
	 *
	 *   Arguments:
	 *     int   ID to use to refer to this event from here on.
	 *     \0    Relative address of this event.
	 *     type  The type of argument the event may send.
	 */
	COMMAND_ADD_EVENT       = 0x04,
	
	
	/**
	 * Slave -> Master:
	 *   Sent to register a property with the server.
	 *
	 *   Arguments:
	 *     int   ID to use to refer to this property from here on.
	 *     \0    Relative address of this property.
	 *     type  The type that this property contains
	 */
	COMMAND_ADD_PROPERTY    = 0x05,
	
	
	/**
	 * Slave -> Master:
	 *   Unregister this action.
	 *
	 *   Arguments:
	 *     int   ID of the action to be unregistered.
	 */
	COMMAND_REMOVE_ACTION   = 0x06,
	
	
	/**
	 * Slave -> Master:
	 *   Unregister this event.
	 *
	 *   Arguments:
	 *     int   ID of the event to be unregistered.
	 */
	COMMAND_REMOVE_EVENT    = 0x07,
	
	
	/**
	 * Slave -> Master:
	 *   Unregister this property.
	 *
	 *   Arguments:
	 *     int   ID of the property to be unregistered.
	 */
	COMMAND_REMOVE_PROPERTY = 0x08,
	
	
	/**
	 * Slave -> Master:
	 *   Signal that an event has been raised and inform all listeners passing on
	 *   all arguments.
	 *
	 *   If this command times out during sending the library will assume that the
	 *   remote system has gone down and will then wait until it has reset.
	 *
	 *   Arguments:
	 *     int   ID of the event raised.
	 *     ?     Any data of the type that was specified during registration.
	 */
	COMMAND_RAISE_EVENT     = 0x09,
	
	
	/**
	 * Master -> Slave:
	 *   Request that a previously registered action be performed on the slave. If
	 *   the action requested is invalid then the slave will ignore it silently.
	 *
	 *   Arguments:
	 *     ?     Any data of the type that was specified during registration.
	 *   Responses:
	 *     ?     Any data of the type that was specified during registration.
	 */
	COMMAND_CALL_ACTION     = 0x0a,
	
	
	/**
	 * Master -> Slave:
	 *   Request that a property is written to with the value given. If the action
	 *   requested is invalid then the slave will ignore it silently.
	 *
	 *   Arguments:
	 *     ?     Data of the type that was specified during registration.
	 */
	COMMAND_SET_PROPERTY    = 0x0b,
	
	
	/**
	 * Master -> Slave:
	 *   Request that the value of a property is retrieved. If the action
	 *   requested is invalid then the slave will ignore it silently.
	 *
	 *   Responses:
	 *     ?     Data of the type that was specified during registration.
	 */
	COMMAND_GET_PROPERTY    = 0x0c,
	
} command_t;

#endif