#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.71])
AC_INIT([LOTR],[0.8.0],[michal.benes@wonderland.cz])
AM_INIT_AUTOMAKE([1.10 foreign])
AC_CONFIG_SRCDIR([src/lotr.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIRS([m4])

# Checks for programs.
AC_PROG_CC
LT_INIT

dnl Check for SDL
AM_PATH_SDL2([],
	[],
	AC_MSG_ERROR([*** SDL2 not found! (libsdl2-dev?)])
)
AC_CHECK_LIB([SDL2_mixer],[Mix_Init])

# Checks for libraries.
PKG_CHECK_MODULES([XML2],[libxml-2.0])
AC_ARG_WITH([hqx],
	[AS_HELP_STRING([--with-hqx], [location of hqx])],
	[AC_DEFINE([HAVE_LIBHQX], [1], [Define if you have libhqx])
	 HQX_CFLAGS="-I$withval/include/hqx"
	 HQX_LIBS="-L$withval/lib -lhqx"
	],
	[AC_CHECK_LIB([hqx],[hqxInit],
	 [AC_DEFINE([HAVE_LIBHQX], [1], [Define if you have libhqx])
	  HQX_CFLAGS="-I/usr/include/hqx"
	  HQX_LIBS="-lhqx"
	 ])
	])
AC_ARG_VAR([HQX_CFLAGS],[CFLAGS for libhqx])
AC_ARG_VAR([HQX_LIBS],[LDFLAGS for libhqx])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h sys/param.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([atexit bzero getcwd memmove memset mkdir])

AC_C_INLINE

AC_ARG_ENABLE([debug],
	[AS_HELP_STRING([--enable-debug], [debugging])],
	[CFLAGS="-g"
	 AC_DEFINE([DEBUG], [1], [Define for debugging])])
AC_ARG_ENABLE([demo],
	[AS_HELP_STRING([--enable-demo], [demo])],
	[AC_DEFINE([DEMO], [1], [Define for demo])])
AC_ARG_ENABLE([extended],
	[AS_HELP_STRING([--enable-extended], [extended])],
	[AC_DEFINE([EXTENDED], [1], [Define for extended])])

AC_CONFIG_FILES([Makefile
                 liblotr/Makefile
                 src/Makefile
                 tools/Makefile])
AC_OUTPUT
