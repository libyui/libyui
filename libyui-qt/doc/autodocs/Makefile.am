#
# Makefile.am for qt/doc/autodocs
#

htmldir = $(docdir)/autodocs

html_DATA = index.html $(wildcard *.html)

CLEANFILES = $(html_DATA)

index.html: $(srcdir)/../../src/include/*.h
	cd $(srcdir)/../../src/include && @ybindir@/ydoc qt *.h
	mv -f $(srcdir)/../../src/include/*.html .
