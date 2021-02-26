#
# Makefile.am for ncurses-pkg/src/
#

AM_CXXFLAGS = -DY2LOG=\"ui-ncurses\"	-DLOCALEDIR=\"${localedir}\"

# plugin, libtool forces 'lib' prefix
# plugin_LTLIBRARIES = libpy2ncurses_pkg.la
plugin_LTLIBRARIES = libpy2ncurses_pkg.la
libpy2ncurses_pkg_la_LDFLAGS = -version-info 2:0

libpy2ncurses_pkg_la_LIBADD =		\
	-L$(plugindir) 			\
	${ZYPP_LIBS}

noinst_HEADERS =			\
	NCPkgTable.h			\
	NCPackageSelectorStart.h	\
        NCPackageSelectorPluginImpl.h	\
	NCPackageSelector.h		\
	NCPkgSelMapper.h		\
	NCPkgStatusStrategy.h		\
	NCPkgMenuDeps.h			\
	NCPkgMenuView.h			\
	NCPkgMenuExtras.h		\
	NCPkgMenuAction.h		\
	NCPkgMenuConfig.h		\
	NCPkgMenuFilter.h		\
	NCPkgMenuHelp.h			\
	NCPkgPackageDetails.h		\
	NCPkgFilterMain.h		\
	NCPkgPopupDeps.h		\
	NCPkgPatchSearch.h		\
	NCPkgPopupDiskspace.h		\
	NCPkgPopupDescr.h		\
	NCPkgPopupTable.h		\
	NCPkgFilterRepo.h		\
	NCPkgFilterSearch.h		\
	NCPkgSearchSettings.h		\
	NCPkgFilterPattern.h		\
	NCPkgFilterLocale.h		\
	NCPkgFilterRPMGroups.h		\
	NCPkgFilterInstSummary.h	\
	NCPkgFilterClassification.h	\
	NCPkgStrings.h			\
	NCZypp.h

libpy2ncurses_pkg_la_SOURCES =		\
	NCPkgTable.cc			\
	NCPackageSelectorStart.cc	\
        NCPackageSelectorPluginImpl.cc	\
	NCPackageSelector.cc		\
	NCPkgSelMapper.cc		\
	NCPkgStatusStrategy.cc		\
	NCPkgMenuDeps.cc		\
	NCPkgMenuView.cc		\
	NCPkgMenuExtras.cc		\
	NCPkgMenuAction.cc		\
	NCPkgMenuConfig.cc		\
	NCPkgMenuFilter.cc		\
	NCPkgMenuHelp.cc		\
	NCPkgPackageDetails.cc		\
	NCPkgFilterMain.cc		\
	NCPkgPopupDeps.cc		\
	NCPkgPatchSearch.cc		\
	NCPkgPopupDiskspace.cc		\
	NCPkgPopupDescr.cc		\
	NCPkgPopupTable.cc		\
	NCPkgFilterRepo.cc		\
	NCPkgFilterSearch.cc		\
	NCPkgSearchSettings.cc		\
	NCPkgFilterPattern.cc		\
	NCPkgFilterLocale.cc		\
	NCPkgFilterRPMGroups.cc		\
	NCPkgFilterInstSummary.cc	\
	NCPkgFilterClassification.cc	\
	NCPkgStrings.cc


INCLUDES =  $(CPPFLAGS) -I$(includedir) -I$(includedir)/yui -I$(includedir)/ncurses ${ZYPP_CFLAGS}

