
from pythonspot import *

NORTH = 0
EAST =  1
SOUTH = 2
WEST =  3
STAY =  5

ACTION_USE =        0
ACTION_SKILL =      1
ACTION_MAGIC =      3
ACTION_MOVE =       5
ACTION_TIMER =      6
ACTION_TRADE =      7
ACTION_ATTACK =     8
ACTION_GET =        9
ACTION_QUESTION = 0xb
ACTION_BUY =      0xc
ACTION_TALK =     0xd


EVERYBODY =    0xf0
LEADER =       0xf4
RINGBEARER =   0xf7

FRODO =        0xa0
SAM =          0xa1

HOBBIT =       0x88
WOMAN_HOBBIT = 0x89

ORC =          0x94
NAZGUL =       0x95

RING =         0x94

def default_action_func(spot):
    pass

class Action:
    type = None
    param = None
    npc = None
    func = default_action_func
    
    def __init__(self, type, param, npc, func):
        self.type = type
        self.param = param
        self.npc = npc
        self.func = func
        if func == None:
            self.func = default_action_func

    def match(self, type = None, param = None, npc = None):        
        return (self.type == None or self.type == type or type == 0xff) and \
               (self.param == None or self.param == param or param == 0xff) and \
               (self.npc == None or self.npc == npc or npc == 0xff)


def default_start(self):
    pass

def default_continue(self):
    return 0


class PythonSpot:

    def __init__(self, map, x, y, w, h):
        self.map = map
        self.x = x
        self.y = y
        self.w = w
        self.h = h

        self.start_func = default_start
        self.cont_func = default_continue

    def start(self):
        self.actions = []
        self.start_func(self)

 
    def cont(self):
        result = self.cont_func(self)
        if result != None: return result
        return 0

    def add_action(self, type=None, param=None, npc=None, func=None):
        self.actions.append(Action(type, param, npc, func))

    def has_action(self, type=None, param=None, npc=None):
        for i in range(len(self.actions)):
            if self.actions[i].match(type, param, npc): return 1
        return 0

    def action(self, type=None, param=None, npc=None):
        for i in range(len(self.actions)):
            if self.actions[i].match(type, param, npc):
                self.actions[i].func(self)
                break
 

emptyspot = PythonSpot(0xffff, 0xffff, 0xffff, 0, 0)

