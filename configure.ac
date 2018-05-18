#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([tetron], [0.0.1], [])
AC_CONFIG_SRCDIR([src/tetron.c])
AC_CONFIG_HEADERS([src/config.h])

AC_CONFIG_AUX_DIR([build-aux]) 

#need this to get $host_os
AC_CANONICAL_HOST
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
# -lm for maths
AC_SEARCH_LIBS([sin], [m], [], [AC_MSG_ERROR([Unable to find sin() function])])
PKG_CHECK_MODULES([SDL], sdl, [], AC_MSG_ERROR([Failed to find SDL]))

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
AC_MSG_NOTICE([This message will show ])
AC_MSG_NOTICE([${host_os}])
AC_MSG_NOTICE([${prefix}])

