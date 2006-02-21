#
# Makefile.am for qt/doc/autodocs
#
# Parameters:
#
#   AUTODOCS_CC: Directories that the C++ autodocs is built from
#                Optional - the default is $(srcdir)/../../src
#   AUTODOCS_DEPS: dependencies of the docs, default is $(AUTODOCS_CC)/*.h
#   AUTODOCS_PARAMS: more params for ydoxygen
#
# see also mvidner's post on yast2-announce on 2006-02-21

# AUTODOCS_CC   = $(srcdir)/../../src/

AUTODOCS_DEPS = $(srcdir)/../../src/include/*.h

# Use this to enable generating autodocs:
#
# include $(top_srcdir)/autodocs-cc.ami
#
# and this to disable it:
#
include $(top_srcdir)/autodocs-cc-off.ami
