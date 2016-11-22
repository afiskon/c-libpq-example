#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([libpq_example], [1.0], [/dev/null])
AC_CONFIG_SRCDIR([libpq_example.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

AM_INIT_AUTOMAKE

AC_CHECK_HEADER([postgresql/libpq-fe.h], [], [
  AC_MSG_ERROR([Unable to find postgresql/libpq-fe.h])
])

AC_CHECK_LIB([pq], [PQlibVersion], [], [
  AC_MSG_ERROR([Unable to find libpq])
])

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
