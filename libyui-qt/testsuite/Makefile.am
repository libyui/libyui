#
# Makefile.am for qt/testsuite
#

AM_CXXFLAGS = -DY2LOG=\"ui-qt\"

AUTOMAKE_OPTIONS = dejagnu

INCLUDES= -I$(srcdir)/../src/include -I$(includedir) -I$(includedir)/yui -I$(QTDIR)/include

clean-local:
	rm -f tmp.err.* tmp.out.* ycp.log ycp.sum site.exp libycp.log libycp.sum site.bak

EXTRA_DIST = README
