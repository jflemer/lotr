
from spot_definitons import *



def init_map(id):
    if id == 0:
        add_character (WOMAN_HOBBIT, x=0x25c, y=0xdc, dir=WEST)
        

#################################################################

spot_0 = emptyspot

##################################################################

spot_1 = PythonSpot(0, 0x224, 0xc0, 0xf, 0xf)

def spot_1_start(self):
    self.enter_num += 1
    if self.enter_num == 1:
        message("Do not enter this spot anymore!")
        self.numsteps = 10
    else:
        message("I said do not enter!!! Now go very far away")
        self.numsteps = 30

    
def spot_1_cont(self):

    teleport(who=LEADER, x=-4, y=0, relative=1, dir=(self.numsteps / 3) % 4)
    self.numsteps -= 1

    return self.numsteps > 0
        
    
spot_1.start_func = spot_1_start
spot_1.cont_func = spot_1_cont
spot_1.enter_num = 0


#############################################################

spot_2 = PythonSpot(0, 0x224, 0xe0, 0xf, 0xf)

def spot_2_start(self):
    if question("Answer Yes or No."):
        message("Your answer was Yes.")
    else:
        message("Your answer was No.")

spot_2.start_func = spot_2_start


#############################################################

spot_3 = PythonSpot(0, 0x1e4, 0xd0, 0xf, 0xf)

def spot_3_start(self):
    if(self.nazgul_not_here):
        teleport(who=NAZGUL, relative=1, x=0x48, y=0, dir=WEST)
    

def spot_3_cont(self):
    # if Nazgul is still not here move him
    if(self.nazgul_not_here):
        (x, y) = get_character_pos(LEADER)
        move(who=NAZGUL, x=x + 12, y=y)
        self.nazgul_not_here = get_character_action(NAZGUL) != STAY
        if not self.nazgul_not_here:
            self.add_action(ACTION_ATTACK, npc=NAZGUL, func=spot_3_attack)
            self.add_action(ACTION_TRADE, param=RING, npc=NAZGUL, \
                             func=spot_3_trade_ring)
            message('The Black Rider says "Give me the Ring or I will kill you."')
            set_text(1, "Give me the Ring or I will kill you.")
            set_text(2, "Why talk. Come with me to Mordor")
            set_character_texts(NAZGUL, greeting=1, default_answer=2)
    #continue only if nazgul is not here
    return self.nazgul_not_here

def spot_3_attack(self):
    set_enemy(NAZGUL)
    combat()
    spot_4.nazgul_dead = 1
    spot_4.actions = []

def spot_3_trade_ring(self):
    message("Sauron recovered his ring again!")
    loose()

spot_3.start_func = spot_3_start
spot_3.cont_func = spot_3_cont
spot_3.nazgul_not_here = 1



#############################################################

spot_4 = PythonSpot(0, 0x1e0, 0xcc, 0x17, 0x17)

def spot_4_start(self):
    if not spot_3.nazgul_not_here and not self.nazgul_dead:
        message("Do not move or I will kill you! Give me what you bear.")
        set_text(1, "Give me the Ring!")
        set_character_texts(NAZGUL, greeting=1)
        self.add_action(ACTION_MOVE, func=spot_3_attack)
        self.add_action(ACTION_TRADE, param=RING, npc=NAZGUL, \
                         func=spot_3_trade_ring)

spot_4.start_func = spot_4_start
spot_4.nazgul_dead = 0


#############################################################

spot_5 = PythonSpot(0, 0x224, 0xd0, 0xf, 0xf)

def spot_5_start(self):
    message("Oh no! Orcs in Shire")
    add_character(ORC, x=-12, y=0, relative=1, dir=EAST)
    add_character(ORC, x=12, y=0, relative=1, dir=WEST)
    add_character(ORC, x=0, y=-12, relative=1, dir=SOUTH)
    add_character(ORC, x=0, y=12, relative=1, dir=NORTH)
    set_enemy(ORC)
    combat()
    pythonspot_disable(5)

spot_5.start_func = spot_5_start



#############################################################

spot_6 = PythonSpot(0, 0x248, 0xc8, 0x28, 0x28)

def spot_6_start(self):
    set_character_name(WOMAN_HOBBIT, "Rosy Twofoot")
    set_text(1, "What a nice weather!")
    set_text(2, "Yes Mr. Baggins, you are right.")
    set_text(3, "My potatoes grow very well.")
    set_text(4, "It is so nice today.")
    set_character_texts(WOMAN_HOBBIT, greeting=1, \
                         questions=(("NEWS", 3), \
                                    ("WEATHER", 4)), default_answer=2)

spot_6.start_func = spot_6_start

