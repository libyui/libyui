#
# Makefile.am for qt/testsuite
#

AM_CXXFLAGS = -DY2LOG=\"ui-qt\"

AUTOMAKE_OPTIONS = dejagnu

INCLUDES= -I$(srcdir)/../src/include -I$(includedir) -I$(includedir)/yui -I$(QTDIR)/include

noinst_PROGRAMS = runqt
runqt_SOURCES = runqt.cc
runqt_LDADD = ../src/liby2qt.la -lqt-mt -lpthread -lyui -lycp -lcrypt -lutil
runqt_LDFLAGS = -L$(QTLIBDIR) -L$(libdir)

ybin_PROGRAMS = y2qt
y2qt_SOURCES = y2qt.cc
y2qt_LDADD = -lqt-mt -lpthread -lyui -ly2 -lycp -lcrypt -lutil
y2qt_LDFLAGS = -L$(QTLIBDIR) -L$(libdir)	\
	-Xlinker --whole-archive		\
	$(plugindir)/libpy2stdio.la		\
	$(plugindir)/libpy2script.la		\
	$(plugindir)/libpy2wfm.la		\
	../src/libpy2qt.la			\
	-Xlinker --no-whole-archive

clean-local:
	rm -f tmp.err.* tmp.out.* ycp.log ycp.sum site.exp libycp.log libycp.sum site.bak

EXTRA_DIST = README
