#ifndef SHET_SOURCE_H
#define SHET_SOURCE_H

#define ASSERT(cond)

#ifndef NUM_ACTIONS
#define NUM_ACTIONS    10
#endif

#ifndef NUM_EVENTS
#define NUM_EVENTS     10
#endif

#ifndef NUM_PROPERTIES
#define NUM_PROPERTIES 10
#endif

#ifndef PING_INTERVAL
#define PING_INTERVAL  2500
#endif

/* Number of times to repeat the reset byte. */
#define LENGTH_OF_RESET_COMMAND 10


namespace SHETSource {
	
	/* This is bad practice but allows these structs to be kept in a
	 * separate file while just being part of this file as far as users
	 * are concerned. */
	#include "SHETSource_commands.h"
	#include "SHETSource_types.h"
	
	typedef enum _status_enum_t {
		STATUS_CONNECTED         = 0x00,
		STATUS_RESETTING         = 0x01,
		STATUS_HIGH_LATENCY      = 0x02,
		STATUS_MALFORMED_COMMAND = 0x04 | STATUS_RESETTING,
		STATUS_IO_FAIL           = 0x08 | STATUS_RESETTING,
		STATUS_READ_FAIL         = 0x10 | STATUS_IO_FAIL,
		STATUS_WRITE_FAIL        = 0x20 | STATUS_IO_FAIL,
	} status_enum_t;
	
	typedef uint8_t status_t;
	
	typedef int action_id_t;
	typedef int event_id_t;
	typedef int property_id_t;
	
	class SHETClient;
	
	template <class id_t> class Local;
	class LocalAction;
	class LocalEvent;
	class LocalProperty;
	
	
	template <class id_t>
	class Local {
		protected:
			SHETClient *client;
			char   *address;
			id_t    id;
		
		public:
			void
			Init(SHETClient *client, id_t id)
			{
				this->client  = client;
				this->id      = id;
				this->address = 0;
			};
			
			bool
			InUse(void)
			{
				return this->address != NULL;
			};
	};
	
	
	
	class LocalAction : public Local<action_id_t> {
		protected:
			void (*callback_vv)(void);
			void (*callback_vi)(int value);
			int  (*callback_iv)(void);
			int  (*callback_ii)(int value);
		
		public:
			void Add(char *address, void (*callback)(void));
			void Add(char *address, void (*callback)(int value));
			void Add(char *address, int  (*callback)(void));
			void Add(char *address, int  (*callback)(int value));
			void Remove(void);
			
			void Register(void);
			void Unregister(void);
			
			void operator () (void);
	};
	
	
	
	class LocalEvent : public Local<event_id_t> {
		public:
			void Add(char *address);
			void Remove(void);
			
			void Register(void);
			void Unregister(void);
			
			void operator () (void);
			void operator () (int);
	};
	
	
	
	class LocalProperty : public Local<property_id_t> {
		protected:
			int   *var;
			void (*set_callback)(int value);
			int  (*get_callback)(void);
		
		public:
			void Add(char *address,
			         void (*set_callback)(int value),
			         int  (*get_callback)(void));
			void Add(char *address, int *var);
			void Remove(void);
			
			void Register(void);
			void Unregister(void);
			
			void Set(void);
			void Get(void);
	};
	
	
	
	
	/**
	 * A SHETSource client object -- handles communication with a SHETSource
	 * server over the Comms link provided to the constructor.
	 *
	 * This interface is designed to roughly mimic the actual SHET API to a
	 * limited extent such that usage to those familiar with SHET should be
	 * reasonably intuitive.
	 */
	class SHETClient {
		protected:
			Comms *comms;
			
			char *address;
			
			int connection_id;
			
			status_t state;
			
			LocalAction   actions    [NUM_ACTIONS];
			LocalEvent    events     [NUM_EVENTS];
			LocalProperty properties [NUM_PROPERTIES];
			
			action_id_t   GetNextActionID(void);
			event_id_t    GetNextEventID(void);
			property_id_t GetNextPropertyID(void);
			
			uint8_t status_led;
			void UpdateStatusLED(void);
			
			void MainLoop(void);
			
			void HandleRequests(void);
			void InitialiseWithServer(void);
			
			void OnRcvReset(void);
			void OnRcvPing(void);
			void OnRcvCallAction(void);
			void OnRcvSetProperty(void);
			void OnRcvGetProperty(void);
			
			long int last_ping;
		
		
		public:
			/**
			 * Set up the connection with the Comms link provided.
			 *
			 * @param comms A Comms object which allows bytes to be sent and recieved
			 *        from a remote SHETSource server.
			 * @param address A relative address at which this client should appear
			 *        under in shet.
			 */
			SHETClient(Comms *comms, char *address);
			
			void Init(void);
			
			/**
			 * Handle requests from the server.
			 * 
			 * Place this somewhere in your mainloop and ensure that your mainloop is
			 * run frequently enough for this device to respond in time to the
			 * server's requests.
			 */
			void DoSHET(void);
		
		public:
			/**
			 * Read from the comms connection and on error also set the status bits.
			 */
			bool Read(uint8_t *buf, int len);
			
			/**
			 * Write to the comms connection and on error also set the status bits.
			 */
			bool Write(uint8_t *buf, int len);
			
			/**
			 * Write a null-terminated string to the comms connection and set status
			 * buts on error.
			 */
			bool Write(char *str);
			
			bool ReadCommand    (uint8_t       *x){return Read ((uint8_t *)x, sizeof(*x));};
			bool ReadInt        (int           *x){return Read ((uint8_t *)x, sizeof(*x));};
			bool ReadActionID   (action_id_t   *x){return Read ((uint8_t *)x, sizeof(*x));};
			bool ReadEventID    (event_id_t    *x){return Read ((uint8_t *)x, sizeof(*x));};
			bool ReadPropertyID (property_id_t *x){return Read ((uint8_t *)x, sizeof(*x));};
			
			bool WriteString    (char          *x){return Write(x);};
			bool WriteCommand   (command_t      x){return Write((uint8_t *)(&x), sizeof(x));};
			bool WriteCommand   (command_t     *x){return Write((uint8_t *)x,    sizeof(*x));};
			bool WriteType      (type_t         x){return Write((uint8_t *)(&x), sizeof(x));};
			bool WriteType      (type_t        *x){return Write((uint8_t *)x,    sizeof(*x));};
			bool WriteInt       (int           *x){return Write((uint8_t *)x,    sizeof(*x));};
			bool WriteActionID  (action_id_t   *x){return Write((uint8_t *)x,    sizeof(*x));};
			bool WriteEventID   (event_id_t    *x){return Write((uint8_t *)x,    sizeof(*x));};
			bool WritePropertyID(property_id_t *x){return Write((uint8_t *)x,    sizeof(*x));};
			
			status_t GetState(void);
			void SetState(status_t val);
			void ORState(status_t val);
			
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
			LocalAction *AddAction(char *address, void (*callback)(void));
			/** Overloaded: See documented version. */
			LocalAction *AddAction(char *address, int  (*callback)(void));
			/** Overloaded: See documented version. */
			LocalAction *AddAction(char *address, void (*callback)(int arg));
			/** Overloaded: See documented version. */
			LocalAction *AddAction(char *address, int  (*callback)(int arg));
			
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
			LocalEvent *AddEvent(char *address);
			
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
			 LocalProperty *AddProperty(char *address,
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
			 LocalProperty *AddProperty(char *address, int *var);
			
			/**
			 * Unregister a property on the server.
			 *
			 * @param property The LocalProperty pointer returned when AddProperty was
			 *        called.
			 */
			void RemoveProperty(LocalProperty *property);
	};
}

#endif
