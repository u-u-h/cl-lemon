ACLOCAL_AMFLAGS=-I m4

lib_LTLIBRARIES = liblmn.la
liblmn_la_SOURCES=lemon-api.cc lemon-api.h
liblmn__laLIBS=-lemon

BUILT_SOURCES= lemon-api.lisp lemon.asd
EXTRA_DIST=

lemon-api.lisp: lemon-api.i lemon-api.h
	@SWIG@  $(CPPFLAGS) -I. -cffi -swig-lisp -generate-typedef $<
