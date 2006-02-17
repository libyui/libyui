#
# Makefile.am for qt/doc/autodocs
#

htmldir = $(docdir)/autodocs

nodist_html_DATA = index.html $(wildcard *.html *.png) doxygen.css @RPMNAME@.tag

CLEANFILES = 	$(html_DATA) 		\
		$(wildcard *.html) 	\
		$(wildcard *.png) 	\
		$(wildcard *.tag) 	\
		doxygen.log 		\
		doxygen.conf 		\
		doxygen.css 		\
		installdox

doxygen.css @RPMNAME@.tag: index.html

index.html: $(srcdir)/../../src/include/*.h
	@ybindir@/ydoxygen PROJECT_NAME=@RPMNAME@
