#
# Makefile.am for ncurses/testsuite
#

AM_CXXFLAGS = -DY2LOG=\"ui-ncurses\"

AUTOMAKE_OPTIONS = dejagnu

PACKAGE = ncurses

INCLUDES = -I../src -I$(includedir) -I$(includedir)/yui

noinst_PROGRAMS = runcurses
runcurses_SOURCES = runcurses.cc
runcurses_LDADD = ../src/liby2ncurses.la -lncurses -lpanel -lpthread \
	-ly2 -lyui -lycp
runcurses_LDFLAGS = -L$(libdir)			\
	-Xlinker --whole-archive                \
	$(plugindir)/libpy2stdio.la             \
	../src/libpy2ncurses.la			\
	-Xlinker --no-whole-archive

ybin_PROGRAMS = y2ncurses
y2ncurses_SOURCES = y2ncurses.cc
y2ncurses_LDADD = -lncurses -lpanel -lpthread -ly2 -lyui -lycp
y2ncurses_LDFLAGS = -L$(libdir)	\
	-Xlinker --whole-archive		\
	$(plugindir)/libpy2stdio.la		\
	$(plugindir)/libpy2script.la		\
	$(plugindir)/libpy2wfm.la		\
	../src/libpy2ncurses.la			\
	-Xlinker --no-whole-archive

clean-local:
	rm -f tmp.err.* tmp.out.* ycp.log ycp.sum site.exp libycp.log \
	libycp.sum site.bak

EXTRA_DIST = README
