m4_define([exttextcat_version_major],[3])
m4_define([exttextcat_version_minor],[4])
m4_define([exttextcat_version_micro],[1])
m4_define([exttextcat_version],[exttextcat_version_major.exttextcat_version_minor.exttextcat_version_micro])

AC_PREREQ([2.68])

AC_INIT([libexttextcat],[exttextcat_version])
AC_CONFIG_MACRO_DIR([m4])
AC_LANG([C])

AM_INIT_AUTOMAKE([1.11 foreign dist-xz dist-bzip2])
AM_MAINTAINER_MODE
AC_CONFIG_HEADERS([src/config.h])
AM_SILENT_RULES([yes])

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_LIBTOOL

dnl Checks for headers
AC_HEADER_STDC
AC_CHECK_HEADERS([inttypes.h stdint.h string.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM

dnl Checks for functions
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([strdup strpbrk])

# ================
# Check for cflags
# ================
AC_ARG_ENABLE([werror],
	[AS_HELP_STRING([--disable-werror], [Treat all warnings as errors, useful for development])],
	[enable_werror="$enableval"],
	[enable_werror=yes]
)
AS_IF([test x"$enable_werror" != "xno"], [
	CFLAGS="$CFLAGS -Werror"
	CXXFLAGS="$CXXFLAGS -Werror"
])
AS_IF([test x"$GCC" = xyes], [
	# Be tough with warnings and produce less careless code
	CFLAGS="$CFLAGS -Wall -pedantic"
	CXXFLAGS="$CXXFLAGS -Wall -pedantic"
])

AC_SUBST([EXTTEXTCAT_MAJOR_VERSION],[exttextcat_version_major])
AC_SUBST([EXTTEXTCAT_MINOR_VERSION],[exttextcat_version_minor])
AC_SUBST([EXTTEXTCAT_MICRO_VERSION],[exttextcat_version_micro])
AC_SUBST([EXTTEXTCAT_VERSION],[exttextcat_version])

AC_CONFIG_FILES([
libexttextcat.pc
Makefile
langclass/Makefile
langclass/LM/Makefile
langclass/ShortTexts/Makefile
src/Makefile
src/exttextcat-version.h
src/test-primary.sh
src/test-secondary.sh
])
AC_OUTPUT

# ==============================================
# Display final informations about configuration
# ==============================================
AC_MSG_NOTICE([
==============================================================================
Build configuration:
	werror:          ${enable_werror}
==============================================================================
])
