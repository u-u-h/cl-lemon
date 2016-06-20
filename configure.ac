AC_INIT([lemon],[0.1.0],[lisp@uuhaus.de])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign -Wall])

m4_ifdef([AM_SILENT_RULES],
         [AM_SILENT_RULES([yes])],
         [AC_SUBST([AM_DEFAULT_VERBOSITY], [1])])

AC_PROG_CC
AC_PROG_CXX
AC_LANG_PUSH([C++])
AX_CXX_COMPILE_STDCXX_11(,[mandatory])
AC_LANG_POP
AM_PROG_AR
LT_INIT

AC_ARG_WITH([lemon],
            [AS_HELP_STRING([--with-lemon],
	      [specify prefix directory for LEMON package.
		 Equivalent to --with-lemon-include=PATH/include
		 plus --with-lemon-lib=PATH/lib])],
	    [lemondir="$withwal"],
	    [])

AC_ARG_WITH([lemon-include],
	[AS_HELP_STRING([--with-lemon-include=PATH],
	 [specify directory for installed LEMON include files])],
	[lemonincdir="$withval"],
	[if test "x$lemondir" != x; then
	    lemonincdir="$lemondir/include"
	 else
	    lemonincdir="$srcdir/lib"
	 fi])

AC_ARG_WITH([lemon-lib],
	[AS_HELP_STRING([--with-lemon-lib=PATH],
	 [specify directory for the installed LEMON library])],
 	[lemonlibdir="$withval"],
	[if test "x$lemondir" != x; then
	    lemonlibdir="$lemondir/lib"
	 else
	    lemonlibdir="$srcdir/lib/build/lemon"
	 fi])

CPPFLAGS="-I$lemonincdir $CPPFLAGS"
if test x"$lemonincdir" = x"$srcdir/lib"; then
   # uninstalled copy, also need build_dir include for config.h; let's hope
   # the user followed cmake conventions and our README and has
   # done the build in lib/build
   CPPFLAGS="-I$srcdir/lib/build $CPPFLAGS"
fi
LDFLAGS="-L$lemonlibdir $LDFLAGS"

AC_LANG_PUSH([C++])
AC_CHECK_HEADERS([lemon/lgf_writer.h])

# AC_SEARCH_LIBS is not C++-friendly, so we roll our own
SAVED_LDFLAGS=$LDFLAGS
LDFLAGS="$LDFLAGS -lemon"
AC_MSG_CHECKING([for LEMON library])

AC_LINK_IFELSE(
  [AC_LANG_PROGRAM([
	#include <lemon/list_graph.h>
		 ], 
    [lemon::ListGraph dummy])],
  [TEST_LIBS="$TEST_LIBS -lemon"] [HAVE_LEMON=1],
  [AC_MSG_ERROR([LEMON library libemon not found.])])
LDFLAGS=$SAVED_LDFLAGS
AC_MSG_RESULT([found])

AC_LANG_POP

dnl for use in .asd.in file 
AC_SUBST(LEMON_LIB_DIR,$lemonlibdir)

AX_PKG_SWIG([1.3.17], [], [AC_MSG_ERROR([SWIG is required to build.])])

AC_CONFIG_FILES([Makefile lemon.asd])
AC_OUTPUT
