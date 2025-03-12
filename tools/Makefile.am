AM_CPPFLAGS = -I../liblotr -DPREFIX='"@prefix@"' $(HQX_CFLAGS) $(SDL_CFLAGS) $(XML2_CFLAGS)

bin_PROGRAMS = \
	dendx \
	getbuildings_ttt \
	getcharacters \
	getcharacters_ttt \
	getobjects \
	getobjects_ttt \
	getprices \
	getsounds_ttt \
	maps2bmp \
	ndxdec \
	ndxenc \
	ripmaps

dendx_LDADD = ../liblotr/liblotr.la
dendx_SOURCES = dendx.c

getbuildings_ttt_LDADD = ../liblotr/liblotr.la
getbuildings_ttt_SOURCES = getbuildings_ttt.c

getcharacters_LDADD = ../liblotr/liblotr.la
getcharacters_SOURCES = getcharacters.c

getcharacters_ttt_LDADD = ../liblotr/liblotr.la
getcharacters_ttt_SOURCES = getcharacters_ttt.c

getobjects_LDADD = ../liblotr/liblotr.la
getobjects_SOURCES = getobjects.c

getobjects_ttt_LDADD = ../liblotr/liblotr.la
getobjects_ttt_SOURCES = getobjects_ttt.c

getprices_LDADD = ../liblotr/liblotr.la
getprices_SOURCES = getprices.c

getsounds_ttt_LDADD = ../liblotr/liblotr.la
getsounds_ttt_SOURCES = getsounds_ttt.c

maps2bmp_LDADD = ../liblotr/liblotr.la $(SDL_LIBS)
maps2bmp_SOURCES = maps2bmp.c

ndxdec_LDADD = ../liblotr/liblotr.la
ndxdec_SOURCES = ndxdec.c

ndxenc_LDADD = ../liblotr/liblotr.la
ndxenc_SOURCES = ndxenc.c

ripmaps_LDADD = ../liblotr/liblotr.la
ripmaps_SOURCES = ripmaps.c

