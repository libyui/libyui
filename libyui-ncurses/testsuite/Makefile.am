#
# Makefile.am for ncurses/testsuite
#

AM_CXXFLAGS = -DY2LOG=\"ui-ncurses\"

AUTOMAKE_OPTIONS = dejagnu

PACKAGE = ncurses

INCLUDES = -I../src -I$(includedir) -I$(includedir)/yui

noinst_PROGRAMS = runcurses utf8test y2ncurses in_wch

# -lzypp
runcurses_SOURCES = runcurses.cc
runcurses_LDADD = ../src/liby2ncurses.la -lncurses -lpanel -lpthread \
	-ly2 -lyui -lycp -lutil

runcurses_LDFLAGS = -L$(libdir)			\
	-Xlinker --whole-archive                \
	../src/libpy2ncurses.la			\
	-Xlinker --no-whole-archive

utf8test_SOURCES = utf8test.cc			\
		NCstring_test.h NCstring_test.cc

utf8test_LDADD = ../src/liby2ncurses.la -lncursesw -lpanelw -lpthread \
	-ly2 -lyui -lycp -lutil

in_wch_SOURCES = in_wch.cc

#in_wch_LDADD = -lncursesw -lpanelw -lpthread -ly2 -lyui -lycp -lutil
in_wch_LDADD = -lncursesw

y2ncurses_SOURCES = y2ncurses.cc
y2ncurses_LDADD = -lncurses -lpanel -lpthread -ly2 -lyui -lycp \
	-lutil

y2ncurses_LDFLAGS = -static -L$(libdir)	\
	-Xlinker --whole-archive		\
	$(plugindir)/libpy2wfm.la		\
	../src/libpy2ncurses.la			\
	-Xlinker --no-whole-archive

clean-local:
	rm -f tmp.err.* tmp.out.* ycp.log ycp.sum site.exp libycp.log \
	libycp.sum site.bak

EXTRA_DIST = README
