#
# Makefile.am for .../agent-XXpkgXX/doc/autodocs
#

htmldir = $(docdir)/autodocs

html_DATA = index.html $(wildcard *.html *.png) doxygen.css @RPMNAME@.tag
CLEANFILES = $(html_DATA) doxygen.log doxygen.conf installdox

index.html: $(srcdir)/../../src/*.h
	@ybindir@/ydoxygen PROJECT_NAME=@RPMNAME@
