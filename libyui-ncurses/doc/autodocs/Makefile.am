#
# Makefile.am for ncurses/doc/autodocs
#

htmldir = $(docdir)/autodocs

html_DATA = index.html $(wildcard *.html *.png) doxygen.css @RPMNAME@.tag

CLEANFILES = $(html_DATA) doxygen.log doxygen.conf installdox

doxygen.css @RPMNAME@.tag: index.html

index.html: $(srcdir)/../../src/*.h
	${YDOXYGEN} PROJECT_NAME=@RPMNAME@ INPUT=$(srcdir)/../../src
