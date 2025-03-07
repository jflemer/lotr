
# Lord of the Ring game engine Makefile for Linux

ifeq ($(PREFIX),)
PREFIX=/usr/local
endif

# Version of SDL to use
SDL_VERSION = 2

# use SDL mixer?
USE_SDL_MIXER = 1

# test for HQX scaler
HQX_INCLUDE = .
USE_HQX = 0
hqx_candidates := /usr/include /usr/include/hqx /usr/local/include /usr/local/include/hqx
hqx_headers := $(foreach dir,$(hqx_candidates),$(wildcard $(dir)/hqx.h))
ifneq "$(hqx_headers)" ""
  USE_HQX = 1
  HQX_INCLUDE = $(dir $(firstword $(hqx_headers)))
endif

#STATIC = --static
STATIC =
CC = gcc
LD = gcc $(STATIC)

HQX_CFLAGS = -I$(HQX_INCLUDE)
HQX_LDFLAGS = -lhqx
ifeq ($(SDL_VERSION),2)
  SDL_CFLAGS := $(shell pkg-config sdl2 --cflags)
  SDL_LDFLAGS := $(shell pkg-config sdl2 --libs $(STATIC) )
else
  SDL_CFLAGS := $(shell pkg-config sdl --cflags)
  SDL_LDFLAGS := $(shell pkg-config sdl --libs $(STATIC) )
endif
XML_CFLAGS := $(shell xml2-config --cflags)
XML_LDFLAGS := $(shell xml2-config --libs $(STATIC) )

INCLUDES = $(SDL_CFLAGS) $(XML_CFLAGS)
LIBRARIES = $(SDL_LDFLAGS) $(XML_LDFLAGS)
CFLAGS = -Wall
CCPARAMS = $(CFLAGS) $(INCLUDES) $(DEFINITIONS)

ifeq ($(USE_SDL_MIXER),1)
  ifeq ($(SDL_VERSION),2)
    LIBRARIES += $(shell pkg-config SDL2_mixer --libs $(STATIC))
  else
    LIBRARIES += $(shell pkg-config SDL_mixer --libs $(STATIC))
  endif
endif

ifeq ($(USE_HQX),1)
INCLUDES += -DUSE_HQX $(HQX_CFLAGS)
LIBRARIES += $(HQX_LDFLAGS)
endif

include Makefile.common
