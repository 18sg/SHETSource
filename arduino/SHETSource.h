#ifndef SHET_SOURCE_H
#define SHET_SOURCE_H

#define ASSERT(cond)

#define NUM_ACTIONS    10
#define NUM_EVENTS     10
#define NUM_PROPERTIES 10


namespace SHETSource {
	
	/* This is bad practice but allows the commands struct to be kept in a
	 * separate file while just being part of this file as far as users
	 * are concerned. */
	#include "SHETSource_commands.h"
	
	typedef enum _status_t {
		STATUS_CONNECTED       = 0x00,
		STATUS_BOOTING         = 0x01,
		STATUS_UNKNOWN_COMMAND = 0x02,
	} status_t;
	
	typedef int action_id_t;
	typedef int event_id_t;
	typedef int property_id_t;
	
	class Client;
	
	class Local;
	class LocalAction;
	class LocalEvent;
	class LocalProperty;
	
	
	/**
	 * We take-over the arduino setup call and so provide this alternative setup
	 * for the user to define. It is called once after the Client has been created
	 * but before it starts its mainloop.
	 */
	void setup(Client *client);
	
	
	/**
	 * We take-over the arduino mainloop and so provide this alternative mainloop
	 * for the user to define. It is called only when the client is connected and
	 * not waiting to reset.
	 *
	 * The user should take care to ensure that this function does not block for a
	 * significant amount of time
	 */
	void loop(Client *client);
	
	
	/**
	 * A SHETSource client object -- handles communication with a SHETSource
	 * server over the Comms link provided to the constructor.
	 *
	 * This interface is designed to roughly mimic the actual SHET API to a
	 * limited extent such that usage to those familiar with SHET should be
	 * reasonably intuitive.
	 */
	class Client {
		protected:
			Comms *comms;
			
			status_t state;
			
			LocalAction   actions    [NUM_ACTIONS];
			LocalEvent    events     [NUM_EVENTS];
			LocalProperty properties [NUM_PROPERTIES];
			
			action_id_t   GetNextActionID(void);
			event_id_t    GetNextEventID(void);
			property_id_t GetNextPropertyID(void);
			
			uint8_t status_led;
			void UpdateStatusLED(void);
			
		
		public:
			/**
			 * Set up the connection with the Comms link provided.
			 *
			 * @param comms A Comms object which allows bytes to be sent and recieved
			 *        from a remote SHETSource server.
			 */
			Client(Comms *comms);
			
			/**
			 * Handle requests from the server.
			 * 
			 * Place this somewhere in your mainloop and ensure that your mainloop is
			 * run frequently enough for this device to respond in time to the
			 * server's requests.
			 */
			bool Sync(void);
		
		public:
			/**
			 * Register an action with the server.
			 *
			 * An action is like an RPC call: once registered it allows a remote SHET
			 * client to call a function on this device (optionally with an argument)
			 * and optionally recieve some returned value.
			 *
			 * @param  address A string containing the relative address of the action
			 *         on SHET.
			 * @param  callback A function pointer to be called when the action is
			 *         called. This function may be passed the argument given in shet
			 *         and may return a value.
			 *
			 * @return Returns a pointer to the LocalAction object which represents
			 *         this action. You will need to keep this refrence in order to
			 *         remove the action using RemoveAction.
			 */
			LocalAction *AddAction(static char *address, void (*callback)(void));
			/** Overloaded: See documented version. */
			LocalAction *AddAction(static char *address, int  (*callback)(void));
			/** Overloaded: See documented version. */
			LocalAction *AddAction(static char *address, void (*callback)(int arg));
			/** Overloaded: See documented version. */
			LocalAction *AddAction(static char *address, int  (*callback)(int arg));
			
			/**
			 * Unregister an action with the server.
			 *
			 * @param action The LocalAction pointer returned when AddAction was
			 *        called.
			 */
			void RemoveAction(LocalAction *action);
			
			
			/**
			 * Register an event on the SHET server.
			 *
			 * An event is a signal you can assert, along with an argument, to any
			 * remote listening SHET clients.
			 *
			 * @param  address A string containing the relative address of the event
			 *         on SHET.
			 *
			 * @return Returns a pointer to the LocalEvent object which represents
			 *         this event. You will need to keep this refrence in order to
			 *         remove the event using RemoveEvent. This object can be called
			 *         in order to raise the event.
			 */
			LocalEvent *AddEvent(static char *address);
			
			/**
			 * Unregister an event on the server.
			 *
			 * @param event The LocalEvent pointer returned when AddEvent was
			 *        called.
			 */
			void RemoveEvent(LocalEvent *event);
			
			
			/**
			 * Register a property on the SHET server.
			 *
			 * A property is a value that can be get'd and set'd by remote SHET
			 * clients.
			 *
			 * @param  address A string containing the relative address of the
			 *         property on SHET.
			 * @param  set_callback A function pointer that will be called when the
			 *         value of the property is being set remotely.
			 * @param  get_callback A function pointer that will be called when the
			 *         value of the property is being requested for reading remotely.
			 *
			 * @return Returns a pointer to the LocalProperty object which represents
			 *         this property. You will need to keep this refrence in order to
			 *         remove the property using RemoveProperty.
			 */
			 LocalProperty *AddProperty(static char *address,
			                           void (*set_callback)(int value),
			                           int  (*get_callback)(void));
			/**
			 * Register a property on the SHET server.
			 *
			 * A property is a value that can be get'd and set'd by remote SHET
			 * clients.
			 *
			 * @param  address A string containing the relative address of the
			 *         property on SHET.
			 * @param  var A pointer to a value which should be set and retrieved when
			 *         requested by remote SHET clients.
			 *
			 * @return Returns a pointer to the LocalProperty object which represents
			 *         this property. You will need to keep this refrence in order to
			 *         remove the property using RemoveProperty.
			 */
			 LocalProperty *AddProperty(static char *address, int *var);
			
			/**
			 * Unregister a property on the server.
			 *
			 * @param property The LocalProperty pointer returned when AddProperty was
			 *        called.
			 */
			void RemoveProperty(LocalProperty *property);
	};
	
	
	
	template <class id_t>
	class Local {
		protected:
			Client *client;
			char   *address;
			id_t    id;
		
		public:
			void Init(Client *client, id_t id);
			bool InUse(void);
	};
	
	
	
	class LocalAction : public Local<action_id_t> {
		protected:
			void (*callback_vv)(void);
			void (*callback_vi)(int value);
			int  (*callback_iv)(void);
			int  (*callback_ii)(int value);
		
		public:
			void Setup(static char *address, void (*callback)(void));
			void Setup(static char *address, void (*callback)(int value));
			void Setup(static char *address, int  (*callback)(void));
			void Setup(static char *address, int  (*callback)(int value));
			void Remove(void);
	};
	
	
	
	class LocalEvent : public Local<event_id_t> {
		public:
			void Setup(static char *address);
			void Remove(void);
	};
	
	
	
	class LocalProperty : public Local<property_id_t> {
		protected:
			int   *var;
			void (*set_callback)(int value);
			int  (*get_callback)(void);
		
		public:
			void Setup(static char *address,
			           void (*set_callback)(int value),
			           int  (*get_callback)(void));
			void Setup(static char *address, int *var);
			
			void Remove(void);
	};
	
}

#endif
