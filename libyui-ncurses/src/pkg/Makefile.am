#
# Makefile.am for ncurses/src/pkg
#

ydata_DATA = pkg_layout.ycp	\
	you_layout.ycp

AM_CXXFLAGS = -DY2LOG=\"ui-ncurses\"	-DLOCALEDIR=\"${localedir}\"
AM_LDFLAGS = -L$(libdir)

# plugin, libtool forces 'lib' prefix
# plugin_LTLIBRARIES = libpy2ncurses_pkg.la
plugin_LTLIBRARIES = libpy2ncurses_pkg.la
libpy2ncurses_pkg_la_LDFLAGS = -version-info 2:0

libpy2ncurses_pkg_la_LIBADD =			\
	-L$(libdir) -lyui			\
	${ZYPP_LIBS}

noinst_HEADERS = $(wildcard *.h)

libpy2ncurses_pkg_la_SOURCES =			\
	NCPackageSelector.h NCPackageSelector.cc		\
	NCPackageSelectorStart.h NCPackageSelectorStart.cc	\
							\
	NCPkgSelMapper.h NCPkgSelMapper.cc		\
	NCPkgStatusStrategy.h NCPkgStatusStrategy.cc	\
	NCPkgPopupDeps.h NCPkgPopupDeps.cc		\
	NCPkgPopupDiskspace.h NCPkgPopupDiskspace.cc	\
	NCPkgPopupDescr.h NCPkgPopupDescr.cc		\
	NCPkgPopupTable.h NCPkgPopupTable.cc		\
	NCPkgPopupSearch.h NCPkgPopupSearch.cc		\
	NCPkgPopupSelection.h NCPkgPopupSelection.cc	\
	NCPkgPopupTree.h NCPkgPopupTree.cc		\
	NCPkgTable.h NCPkgTable.cc			\
	NCPkgNames.h NCPkgNames.cc			\
	NCZypp.h


INCLUDES = -I$(srcdir)/.. -I$(includedir) -I$(includedir)/yui ${ZYPP_CFLAGS}

EXTRA_DIST	= $(ydata_DATA)
