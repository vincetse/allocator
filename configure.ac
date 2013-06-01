#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([allocator], [0.1.0], [thelazyenginerd@gmail.com])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/buffer_allocator_test.cpp])
AC_CONFIG_HEADERS([config.h])

CXXFLAGS+=" -Wall -Werror -std=c++0x -fPIC"
CPPFLAGS+=" -I../include"
CPPFLAGS+=" -I/usr/local/include"
LDFLAGS+=" -L/usr/local/lib"

# Checks for programs.
AC_PROG_CXX

# Checks for libraries.
#AC_CHECK_LIB([boost_unit_test_framework], [main])

# Checks for header files.
AC_LANG_PUSH([C++])
AC_FUNC_ALLOCA
AC_CHECK_HEADERS([boost/test/unit_test.hpp], [], [AC_MSG_ERROR(You need the Boost.Test library.)])
AC_LANG_POP([C++])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.

AC_CONFIG_FILES([
    Makefile
    src/Makefile
])

AC_OUTPUT
