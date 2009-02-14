	
#     pythonspot.pyx
#     Python scripting module
#
#
#     Lord of the Rings game engine
#     
#     Copyright (C) 2004  Michal Benes
# 
#     Lord of the Rings game engine is free software;
#     you can redistribute it and/or
#     modify it under the terms of the GNU Lesser General Public
#     License as published by the Free Software Foundation; either
#     version 2.1 of the License, or (at your option) any later version.
# 
#     This code is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#     Lesser General Public License for more details.
# 
#     You should have received a copy of the GNU Lesser General Public
#     License along with this code; if not, write to the Free Software
#     Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 



"""
Python scripting for The Lord of the Rings game engine
"""

import sys

# system libraries
cdef extern from "Python.h":

    # embedding funcs
    void Py_Initialize()
    void Py_Finalize()
    object PyImport_Import(object)
    object PyObject_GetAttr(object, object)


cdef extern from "stdlib.h":
    void exit(int)

cdef extern from "string.h":
    char *strncpy(char*, char*, int)


# lord includes
cdef extern from "cartoon.h":
    void playcartoon(char*)

cdef extern from "character.h":
    ctypedef struct Character:
        int direction
        int action
        int x
        int y
        int life
        int end
        char name[20]
        int pythontexts
        char texts[169]
    int character_get_talk_len()

cdef extern from "combat.h":
    void combat_proceed_frames()
    void combat_start()

cdef extern from "gui.h":
    void gui_message(char*, int)
    void gui_question(char*)
    void gui_proceed_frames()

cdef extern from "game.h":
    void game_leader_teleport(int, int, int, int, int)
    Character *game_get_leader()

cdef extern from "spot.h":
    ctypedef struct CommandSpot:
        int flag
    Character *spot_character_get(int)
    int spot_question_letter(char)
    
cdef extern from "map.h":
    void map_add_pythonspot(int, int, int, int, int)
    void map_character_teleport(Character*, int, int, int, int, int)
    int map_character_move(int, int, int, int)	
    void map_attacking_character(int)
    void map_add_character(Character*)
    CommandSpot* map_get_pythonspot(int)

cdef extern from "utils.h":
    char *lord_data_directory()



pythonspots = []
cdef int pythonspots_num
cdef int if_result
cdef char *pythontexts[256]


def print_error(text, ex=None):
    print "lord: %s" % (text)
    if ex != None:
        print ex
    exit(1)


#
#initialize the system
#
cdef public void pythonspot_init():

    global spots, python_spots_num, pythonspots

    sys.path.append(lord_data_directory())

    try:
        spots = PyImport_Import("spots")
    except Exception, e:
        print_error("error loading pythonmodule spots.py or spot_definitons.py", e)
        
    try:
        pythonspots_num = 0
        while 1:
            spot = PyObject_GetAttr(spots, "spot_%d" % pythonspots_num)
            map_add_pythonspot(spot.map, spot.x, spot.y, spot.w, spot.h)
            pythonspots.append(spot)
            pythonspots_num = pythonspots_num + 1
    except Exception:
        pass

    for i in range(256):
        pythontexts[i] = ""
            
            

#
# shut down the sytem
#
cdef public void pythonspot_close():
    pass

#
# start python spot
#
cdef public void pythonspot_start(int id):
    try:
        pythonspots[id].start()
    except Exception, e:
        print_error("error running python spot id=%d" % id, e)

#
# continue the running python spot
# return if the spot still runs
#
cdef public int pythonspot_continue(int id):
    try:
        return pythonspots[id].cont()
    except Exception, e:
        print_error("error running python spot id=%d" % id, e)

    return 0

#
# return if the spot has the specified action
#
cdef public int pythonspot_has_action(int id, int type, int param, int npc):
    return pythonspots[id].has_action(type, param, npc)

#
# perform the specified action
#
cdef public void pythonspot_action(int id, int type, int param, int npc):
     pythonspots[id].action(type, param, npc)




#
# sets result of an if command
#
cdef public void pythonspot_if_result(int result):
    global if_result	
    if_result = result	


#
# return pythontext
#
cdef public char *pythonspot_get_text(int id):
    if id < 0 or id > 255: return ""
    return pythontexts[id]


#
# initialize a map
#
cdef public void pythonspot_init_map (int id):
    spots.init_map(id)


#
#
# Functions which can be called from the spots
#
###########################################################################

#
# You loose the game
# no return
#
def loose():
    playcartoon("cart10")
    exit(0)

#
# return character action
#
def get_character_action(who):
    cdef Character *character
    character = spot_character_get(who)
    return character.action

#
# set character action
# no return value
#
def set_character_action(who, value):
    cdef Character *character
    character = spot_character_get(who)
    character.action = value

#
# return character position as tuple
#
def get_character_pos(who):
    cdef Character *character
    character = spot_character_get(who)
    return (character.x, character.y)

#
# set character position
# no return value
#
def set_character_pos(who, x=None, y=None):
    cdef Character *character
    character = spot_character_get(who)
    if x != None: character.x = x
    if y != None: character.y = y



#
# show message
# no return value
#
def message(text):
    gui_message(text, 0)
    gui_proceed_frames()	

#
# ask a Yes/No question
# returns 1 if the answer was Yes
# returns 0 otherwise
#
def question(text):
    gui_question(text)
    gui_proceed_frames()	
    return if_result	


#
# teleport character
# no return value
#
def teleport(who, x, y, relative=0, dir=None, map=None):
    cdef Character *character
    if map == None: map = 0xff
    if dir == None: dir = 0xff
    if x < 0: x = x + 0xffff
    if y < 0: y = y + 0xffff    
    if who != 0xf0:
        character = spot_character_get(who)
    # 0xf0 means whole party in the original data
    if who == 0xf0 or character == game_get_leader():
        game_leader_teleport(relative, x, y, dir, map)
    elif character != NULL:
        map_character_teleport(character, relative, x, y, dir, map) 


#
# move character to position (x,y) and set final direction
# this function actually makes only one step
# test character.action==STAY to check if character is at the destination
# no return value
#
def move(who, int x, int y, direction=None):
    cdef Character *character
    cdef int dir
    character = spot_character_get(who)

    if direction == None:
        dir = character.direction
    else:
        dir = direction

    if dir < 0 or dir > 3:
        print_error("Wrong direction dir=%d" % dir)

    map_character_move(who, x, y, dir)



#
# set character as enemy
# no return value
#
def set_enemy(who):
    map_attacking_character(who)


#
# start combat
# no return value
#
def combat():
    combat_start()
    combat_proceed_frames()	



#
# play cartoon
# no return value
#
def cartoon(name):
    playcartoon(name)



#
# add character
# if relative is set then the positions are relative to leader
# no return value
#
def add_character(who, x, y, dir, relative=0):
    cdef Character *character
    cdef Character *leader
    character = spot_character_get(who) 
    leader = game_get_leader() 
    if relative:
        character.x = leader.x + x
        character.y = leader.y + y
    else:
        character.x = x
        character.y = y
    character.direction = dir
    map_add_character(character)



#
# enable python spot
# no return value
#
def pythonspot_enable(id):
    cdef CommandSpot *spot
    spot = map_get_pythonspot(id)
    spot.flag = 1


#
# disable python spot
# no return value
#
def pythonspot_disable(id):
    cdef CommandSpot *spot
    spot = map_get_pythonspot(id)
    spot.flag = 0


#
# set character name
# no return value
#
def set_character_name(who, name):
    cdef Character *character
    character = spot_character_get(who)
    strncpy(character.name, name, 20)
    character.name[20] = 0


#
# set pythonspot texts
# no return value
#
def set_text(id, text):
    cdef int n
    global pythontexts
    n = id
    if n < 0 or n > 255: return
    pythontexts[n] = text

#
# set character texts
# questions parameter is list of tuples (question, inswer_id)
# questions must be in uppercase
# no return value
#
def set_character_texts(who, greeting=None, \
	                 questions=None, default_answer=None):
    cdef Character *character
    cdef int pos
    cdef int textslen

    character = spot_character_get(who)
    character.pythontexts = 1

    if greeting != None:
        character.texts[0] = greeting

    textslen = character_get_talk_len()	
    pos = 1
    if questions != None:
        for i in range(len(questions)):	

            if pos > textslen - 20:
                print "lord: too many questions in spot script"
                break
            (question,text) = questions[i];

            for j in range(min(10, len(question))):
                if not spot_question_letter(ord(question[j])):
                    print "lord: invalid question character '%s'" % question[j]
                    break
                character.texts[pos] = ord(question[j])
                pos = pos + 1

            character.texts[pos] = text
            pos = pos + 1
            

            

    if default_answer != None:
        character.texts[pos] = 0
        pos = pos + 1
        character.texts[pos] = default_answer
        pos = pos + 1

    character.texts[pos] = 0xff

