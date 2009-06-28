CC = gcc
LD = gcc

CFLAGS = -Wall -O2
INCLUDES = `sdl-config --cflags` `xml2-config --cflags`
LIBRARIES = `sdl-config --libs` `xml2-config --libs`
CCPARAMS = $(CFLAGS) $(INCLUDES) $(DEFINITIONS)

TARGETS = ndxdec ndxenc

all: $(TARGETS)

clean:
	rm -f $(TARGETS) *.o

ndxdec: ndxdec.o ../archive.o ../utils.o
	$(LD) $(LIBRARIES) $? -o ndxdec

ndxdec.o: ndxdec.c
	$(CC) $(CCPARAMS) -c $< -o $@

ndxenc: ndxenc.o 
	$(LD) $(LIBRARIES) $? -o ndxenc

ndxenc.o: ndxenc.c
	$(CC) $(CCPARAMS) -c $< -o $@
