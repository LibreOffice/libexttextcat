dnl Process this file with autoconf to produce a configure script.
AC_PREREQ([2.65])

AC_INIT([libexttextcat], [3.0])
AC_CONFIG_MACRO_DIR([m4])
AC_LANG([C])

AM_INIT_AUTOMAKE([1.11 foreign dist-xz dist-bzip2])
AM_CONFIG_HEADER([src/config.h])
AM_SILENT_RULES([yes])

dnl Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_LIBTOOL

dnl Checks for headers
AC_FUNC_ALLOCA
AC_HEADER_STDC
AC_CHECK_HEADERS([inttypes.h limits.h stdlib.h string.h sys/time.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM

dnl Checks for functions
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([gettimeofday memset strchr strdup strpbrk])

dnl Checks for libraries
AC_CONFIG_FILES([
libtextcat.pc
Makefile
langclass/Makefile
langclass/LM/Makefile
langclass/ShortTexts/Makefile
src/Makefile
])
AC_OUTPUT
