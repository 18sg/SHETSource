import types

class FSM(object):
	
	def __init__(self, end_state_callback = None):
		# Start the FSM on the initial_state
		self.state = self.initial_state
		
		# When the last state is reached, this function should be called and it will
		# emit the next state to be used. When using sub-fsms this callback is a
		# function which accepts one argument which is passed back to the yielding
		# state. If unspecified this defaults to a function which always returns the
		# state to the initial_state.
		self.end_state_callback = (end_state_callback if end_state_callback is not None
		                           else (lambda *x, **y: self.initial_state))
	
	
	def process(self, string):
		"""
		Cycle the FSM for each char in the provided string.
		"""
		for char in string:
			# Execute this state
			next_state = self.state(char)
			self.state = next_state


def state(f):
	"""
	Decorator which makes a function into a valid FSM state.
	
	If the decorated function returns another state, this will be treated as the
	next state.
	
	If None is returned then the the state will be unchanged (i.e. that state will
	be the next state).
	
	If a generator is returned then the values yielded are assumed to be FSM
	Classes (not instances) which will be executed to completion and the member
	variable 'value' will be sent back through the yield.
	"""
	
	def decorator(self, *args, **kwargs):
		ret_val = f(self, *args, **kwargs)
		
		if ret_val is None:
			# Nothing was returned so the next state is just the same
			return types.MethodType(f, self, self.__class__)
			
		elif hasattr(ret_val, "next") and hasattr(ret_val, "send"):
			# A generator was returned
			generator = ret_val
			
			def sub_fsm_end_state(value):
				"""
				Called at the end of a sub-fsm
				"""
				try:
					# Send the value found by the completed FSM back to the generator that
					# provided it, fetching the next FSM Class to use from the next yield.
					next_sub_fsm_class = generator.send(value)
					
					# Instanciate the provided FSM class giving this function as the end
					# state so that when it finishes the value from the FSM can be sent
					# like above.
					sub_fsm = next_sub_fsm_class(sub_fsm_end_state)
					
					# Return the starting state of the newly created FSM as the next
					# state.
					return sub_fsm.initial_state
				
				except StopIteration:
					# When the generator has finished set the state to the end state.
					return self.end_state_callback()
			
			# Fetch the first sub-fsm from the generator and run it with the
			# sub_fsm_end_state wrapper as the end state.
			next_sub_fsm_class = generator.next()
			return next_sub_fsm_class(sub_fsm_end_state).initial_state
			
		else:
			# The type returned is (assumed to be) a state
			return ret_val
	
	return decorator



def run_tests():
	class MainFSM(FSM):
		
		def __init__(self, *args, **kwargs):
			self.return_nothing_toggle = True
			
			self.initial_state = self.start
			FSM.__init__(self, *args, **kwargs)
		
		@state
		def start(self, value):
			print
			print "MainFSM.start: %s"%value
			return self.return_nothing
		
		@state
		def return_nothing(self, value):
			print "MainFSM.return_nothing: %s"%value
			
			self.return_nothing_toggle = not self.return_nothing_toggle
			
			if self.return_nothing_toggle:
				return self.yielder
		
		@state
		def yielder(self, value):
			print "MainFSM.yielder: %s"%value
			print "MainFSM.yielder: Sub-FSM: %s"%(yield SubFSM)
			print "MainFSM.yielder: Sub-FSM: %s"%(yield SubFSM)
		
		initial_state = return_nothing
	
	class SubFSM(FSM):
		
		@state
		def start(self, value):
			print "SubFSM.start: %s"%value
			
			return self.end_state_callback("Sent Back '%s'"%value)
		
		initial_state = start
	
	main_fsm = MainFSM()
	main_fsm.process("abcdefabcdef")


if __name__ == "__main__":
	run_tests()
