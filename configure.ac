#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([C_Interpreter], [1.0], [rob121618@gmail.com])
AC_CONFIG_SRCDIR([src/main.c])
#AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AM_INIT_AUTOMAKE([subdir-objects])
#AC_PROG_CXX
AC_PROG_CC_C99
AC_PROG_MAKE_SET

# Checks for libraries.
dnl AC_CHECK_LIB(cunit, CU_initialize_registry)

# Checks for header files.
AC_CHECK_HEADERS([limits.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_SIZE_T
AC_CHECK_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memchr memmove memset sqrt strchr strstr])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
