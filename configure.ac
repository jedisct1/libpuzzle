#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(libpuzzle, 0.11, bugs@pureftpd.org)
AC_CONFIG_SRCDIR([src/puzzle.h])
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE([1.9 dist-bzip2])
AM_MAINTAINER_MODE

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PATH_PROG(GDLIBCONFIG, [gdlib-config])
CPPFLAGS="$CPPFLAGS -D_GNU_SOURCE=1" 
CPPFLAGS="$CPPFLAGS `$GDLIBCONFIG --cflags`"
LDFLAGS="$LDFLAGS `$GDLIBCONFIG --ldflags`"
LDADD="$LDADD `$GDLIBCONFIG --libs`"

# Checks for libraries.

AC_CHECK_LIB([gd], [gdImageCreateFromGd2],,
             AC_ERROR([libgd2 development files not found]))

# Checks for header files.
AC_HEADER_STDC
AM_PROG_LIBTOOL
AC_CHECK_HEADERS([limits.h memory.h stddef.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_OFF_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_MEMCMP
AC_CHECK_FUNC([floor], ,[AC_CHECK_LIB([math], [floor])])
AC_CHECK_FUNC([round], ,[AC_CHECK_LIB([math], [round])])
AC_CHECK_FUNCS([strtoul])

AC_SUBST([MAINT])

AC_CONFIG_FILES([Makefile
                 man/Makefile
                 src/Makefile
                 src/pics/Makefile
                 php/Makefile
                 php/libpuzzle/Makefile
                 php/libpuzzle/include/Makefile
                 php/libpuzzle/modules/Makefile
                 php/libpuzzle/build/Makefile
                 php/libpuzzle/tests/Makefile
                 php/libpuzzle/tests/pics/Makefile
                 php/examples/Makefile
                 php/examples/similar/Makefile                 
                 ])
AC_OUTPUT

AC_MSG_NOTICE([+-------------------------------------------------------+])
AC_MSG_NOTICE([| You can subscribe to the Libpuzzle users mailing-list |])
AC_MSG_NOTICE([| to ask for help and to stay informed of new releases. |])
AC_MSG_NOTICE([| Go to http://libpuzzle.pureftpd.org/ml/ now!          |])
AC_MSG_NOTICE([+-------------------------------------------------------+])
