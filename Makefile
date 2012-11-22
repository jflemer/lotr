
# Lord of the Ring game engine Makefile for Linux

ifeq ($(PREFIX),)
PREFIX=/usr/local
endif

# use SDL mixer?
USE_SDL_MIXER=1

# test for HQX scaler
HQX_INCLUDE=/usr/include/hqx
ifeq ($(wildcard $(HQX_INCLUDE)),) 
    USE_HQX=0
else
    USE_HQX=1
endif

CC = gcc
LD = gcc
LD_STATIC = gcc --static

HQX_CFLAGS = -I$(HQX_INCLUDE)
HQX_LDFLAGS = -lhqx
SDL_CFLAGS := $(shell pkg-config sdl --cflags)
SDL_LDFLAGS := $(shell pkg-config sdl --libs)
XML_CFLAGS := $(shell xml2-config --cflags)
XML_LDFLAGS := $(shell xml2-config --libs)

INCLUDES = $(SDL_CFLAGS) $(XML_CFLAGS)
LIBRARIES = $(SDL_LDFLAGS) $(XML_LDFLAGS)
STATIC_LIBRARIES = `sdl-config --static-libs` `xml2-config --libs --static`
CFLAGS = -Wall
CCPARAMS = $(CFLAGS) $(INCLUDES) $(DEFINITIONS)

ifeq ($(USE_SDL_MIXER),1)
LIBRARIES += -lSDL_mixer
STATIC_LIBRARIES += -lSDL_mixer
endif

ifeq ($(USE_HQX),1)
INCLUDES += -DUSE_HQX $(HQX_CFLAGS)
LIBRARIES += $(HQX_LDFLAGS)
endif

include Makefile.common
