#!/usr/bin/python


from error import NotConnected


class ClientSHET(object):
	"""
	A SHET wrapper for the Client class.
	"""
	
	def _clear_shet(self):
		"""Clear all SHET records for this client."""
		for prop_id   in self._properties.keys() : self.remove_property(prop_id)
		for event_id  in self._events.keys()     : self.remove_event(event_id)
		for action_id in self._actions.keys()    : self.remove_action(action_id)
	
	
	def add_property(self, id, path, set_callback, get_callback):
		if self.connected:
			prop = self.router.shet.add_property(path, set_callback, get_callback)
			self._properties[id] = prop
			return prop
		else:
			raise NotConnected()
	def remove_property(self, prop_id):
		try:
			self.router.shet.remove_property(self._properties[prop_id])
		except Exception:
			pass
		finally:
			del self._properties[prop_id]
	
	
	def add_action(self, id, path, callback):
		if self.connected:
			action = self.router.shet.add_action(path, callback)
			self._actions[id] = action
			return action
		else:
			raise NotConnected()
	def remove_action(self, action_id):
		try:
			self.router.shet.remove_action(self._actions[action_id])
		except Exception:
			pass
		finally:
			del self._actions[action_id]
	
	
	def add_event(self, id, path):
		if self.connected:
			event = self.router.shet.add_event(path)
			self._events[id] = event
			return event
		else:
			raise NotConnected()
	def remove_event(self, event_id):
		try:
			self.router.shet.remove_event(self._events[event_id])
		except Exception:
			pass
		finally:
			del self._events[event_id]
