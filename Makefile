
# Lord of the Ring game engine Makefile for Linux

ifeq ($(PREFIX),)
PREFIX=/usr/local
endif

# use SDL mixer?
USE_SDL_MIXER=1

CC = gcc
LD = gcc
LD_STATIC = gcc --static

INCLUDES = `sdl-config --cflags` `xml2-config --cflags`
LIBRARIES = `sdl-config --libs` `xml2-config --libs`
STATIC_LIBRARIES = `sdl-config --static-libs` `xml2-config --libs --static`
CFLAGS = -Wall
CCPARAMS = $(CFLAGS) $(INCLUDES) $(DEFINITIONS)

ifeq ($(USE_SDL_MIXER),1)
LIBRARIES += -lSDL_mixer
STATIC_LIBRARIES += -lSDL_mixer
endif

include Makefile.common
