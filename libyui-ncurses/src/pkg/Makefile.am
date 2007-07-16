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

noinst_HEADERS =			\
	NCPackageSelector.h		\
	NCPkgSelMapper.h		\
	NCPkgStatusStrategy.h		\
	NCPkgPopupDeps.h		\
	NCPkgPopupDiskspace.h		\
	NCPkgPopupDescr.h		\
	NCPkgPopupTable.h		\
	NCPkgPopupRepo.h		\
	NCPkgPopupSearch.h		\
	NCPkgPopupSelection.h		\
	NCPkgPopupTree.h		\
	NCPkgNames.h			\
	NCZypp.h

libpy2ncurses_pkg_la_SOURCES =		\
	NCPackageSelector.cc		\
	NCPkgSelMapper.cc		\
	NCPkgStatusStrategy.cc		\
	NCPkgPopupDeps.cc		\
	NCPkgPopupDiskspace.cc		\
	NCPkgPopupDescr.cc		\
	NCPkgPopupTable.cc		\
	NCPkgPopupRepo.cc		\
	NCPkgPopupSearch.cc		\
	NCPkgPopupSelection.cc		\
	NCPkgPopupTree.cc		\
	NCPkgNames.cc


INCLUDES = -I$(srcdir)/.. -I$(includedir) -I$(includedir)/yui ${ZYPP_CFLAGS}

EXTRA_DIST	= $(ydata_DATA)
