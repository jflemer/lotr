
# Lord of the Ring game engine Makefile

LOTR_MAKE_PROCESSED=1

ifeq ($(LOTR_MAKE_CONFIGURED),)
# Call platform specific configuration from here
# include Makefile.os4
endif

ifeq ($(PREFIX),)
PREFIX=/usr/local
endif

# set this to 1 if you want debug version (including cheats)
DEBUG=0

# set this to 1 if you want extended version
# currently only for testing (use together with debug)
EXTENDED=0


# do the configuration only if we have not been called from platform specific script
ifeq ($(LOTR_MAKE_CONFIGURED),)

# use SDL mixer?
ifeq ($(USE_SDL_MIXER),)
USE_SDL_MIXER=1
endif

CC = gcc
LD = gcc
LD_STATIC = gcc --static

INCLUDES = `sdl-config --cflags` `xml2-config --cflags`
LIBRARIES = `sdl-config --libs` `xml2-config --libs`
STATIC_LIBRARIES = `sdl-config --static-libs` `xml2-config --libs --static`
CFLAGS = -Wall
CCPARAMS = $(CFLAGS) $(INCLUDES) $(DEFINITIONS)
endif

DEFINITIONS = -DPREFIX=\"$(PREFIX)\"

HEADERFILES = archive.h av.h cartoon.h character.h combat.h credit.h lord.h lord_sdl.h map.h midi.h game.h graphics.h gui.h init.h object.h shape.h sound.h spot.h timing.h utils.h
PLAYCARTOONOBJ = archive.o cartoon.o lord_sdl.o midi.o graphics.o sound.o timing.o utils.o
PLAYAVOBJ = archive.o av.o lord_sdl.o midi.o graphics.o timing.o utils.o
OBJFILES = archive.o av.o cartoon.o character.o combat.o lord_sdl.o map.o midi.o game.o gui.o graphics.o init.o object.o shape.o sound.o spot.o timing.o utils.o

ifeq ($(USE_SDL_MIXER),1)
LIBRARIES += -lSDL_mixer
STATIC_LIBRARIES += -lSDL_mixer
DEFINITIONS += -DHAVE_SDL_MIXER
endif

ifeq ($(EXTENDED),1)
OBJFILES += $(OBJFILES_C) pythonspot.o
DEFINITIONS += -DEXTENDED=1
PYVERSION := $(shell python setup.py version)
PYPREFIX := $(shell python setup.py prefix)
LIBRARIES += -lpython$(PYVERSION)
INCLUDES += -I$(shell python setup.py include)
HEADERFILES += pythonspot.h
endif

ifeq ($(DEBUG),1)
DEFINITIONS += -DDEBUG
CFLAGS += -ggdb -O0
else
CFLAGS += -g0 -O2 -funroll-loops
endif

all: lord

clean:
	rm -f *.o *~ core.* pythonspot.c playcartoon lord lorddemo \
		pythonspot.pxi pythonspot.h playav


lord: $(OBJFILES) lord.o
	$(LD) $(OBJFILES) $(LIBRARIES) lord.o -o lord

lord.static: $(OBJFILES) lord.o
	$(LD_STATIC) $(OBJFILES) $(STATIC_LIBRARIES) lord.o -o lord.static


playcartoon: $(PLAYCARTOONOBJ) playcartoon.o
	$(LD) $(PLAYCARTOONOBJ) $(LIBRARIES) playcartoon.o -o playcartoon


playav: $(PLAYAVOBJ) playav.o
	$(LD) $(PLAYAVOBJ) $(LIBRARIES) playav.o -o playav



object.o: object.c $(HEADERFILES) object_names.h object_names_ttt.h object_prices.h
	$(CC) $(CCPARAMS) -c object.c

shape.o: shape.c shapes_param.h shapes_param_ttt.h $(HEADERFILES)
	$(CC) $(CCPARAMS) -c shape.c

sound.o: sound.c sounds_cd.h $(HEADERFILES)
	$(CC) $(CCPARAMS) -c sound.c


%.o: %.c $(HEADERFILES) Makefile
	$(CC) $(CCPARAMS) -c $< -o $@

pythonspot.c: pythonspot.pyx
	pyrexc $<

pythonspot.h: pythonspot.pyx
	pyrexc $<


testmap: testmap.c
	$(CC) $(CCPARAMS) $(LIBRARIES) testmap.c -o testmap
