import types

class Fsm(object):

    def __init__(self):
        self.state = self.initial_state


    def process(self, string):
        for char in string:
            self.state = self.state(char) or self.state


def state(f):
    def g(*args, **kwargs):
        retval = f(*args, **kwargs)
        if retval is None:
            return types.MethodType(f, args[0], args[0].__class__)
        else:
            return retval
    return g


