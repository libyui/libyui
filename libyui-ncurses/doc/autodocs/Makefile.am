#
# Makefile.am for ncurses/doc/autodocs
#

htmldir = $(docdir)/autodocs

html_DATA = index.html $(wildcard *.html)

CLEANFILES = $(html_DATA)

index.html: $(srcdir)/../../src/*.h
	cd $(srcdir)/../../src && @ybindir@/ydoc ncurses *.h
	mv -f $(srcdir)/../../src/*.html .
