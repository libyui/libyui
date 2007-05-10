#
# Makefile.am for qt/src/pkg
#

SUBDIRS = icons

AM_CXXFLAGS = -DY2LOG=\"ui-qt\"				\
	-DLOCALEDIR=\"${localedir}\"			\
	-DQT_LOCALEDIR=\"$(QTDIR)/translations\"	\
	-DICONDIR=\"${themedir}/current\"		\
	-DTHEMEDIR=\"${themedir}/current\"		\
	-DHELPIMAGEDIR=\"${imagedir}\"			\
	-DQTLIBDIR=\"$(QTLIBDIR)\"			\
	-Wno-format-nonliteral				\
	-fmessage-length=0


MOC = $(QTDIR)/bin/moc

# plugin, libtool forces 'lib' prefix
# plugin_LTLIBRARIES = libpy2qt_pkg.la
plugin_LTLIBRARIES = libpy2qt_pkg.la
libpy2qt_pkg_la_LDFLAGS = -version-info 2:0

libpy2qt_pkg_la_LIBADD =			\
	-L$(libdir) -lyui			\
	-L$(QTLIBDIR) -lqt-mt			\
	-lX11 -lXext -lSM -lICE			\
	${ZYPP_LIBS}

noinst_HEADERS = $(wildcard *.h)

libpy2qt_pkg_la_SOURCES =			\
	YQPackageSelector.cc			\
	YQPackageSelectorBase.cc		\
	YQPackageSelectorHelp.cc		\
	YQPatternSelector.cc			\
	YQSimplePatchSelector.cc		\
						\
	YQPkgChangeLogView.cc			\
	YQPkgChangesDialog.cc			\
	YQPkgConflictDialog.cc			\
	YQPkgConflictList.cc			\
	YQPkgDependenciesView.cc		\
	YQPkgDescriptionDialog.cc		\
	YQPkgDescriptionView.cc			\
	YQPkgDiskUsageList.cc			\
	YQPkgDiskUsageWarningDialog.cc		\
	YQPkgFileListView.cc			\
	YQPkgGenericDetailsView.cc		\
	YQPkgInstSrcFilterView.cc		\
	YQPkgInstSrcList.cc			\
	YQPkgLangList.cc			\
	YQPkgList.cc				\
	YQPkgObjList.cc				\
	YQPkgPatchFilterView.cc			\
	YQPkgPatchList.cc			\
	YQPkgPatternList.cc			\
	YQPkgProductDialog.cc			\
	YQPkgProductList.cc			\
	YQPkgRpmGroupTagsFilterView.cc		\
	YQPkgSearchFilterView.cc		\
	YQPkgSelDescriptionView.cc		\
	YQPkgSelList.cc				\
	YQPkgSelMapper.cc			\
	YQPkgStatusFilterView.cc		\
	YQPkgTechnicalDetailsView.cc		\
	YQPkgTextDialog.cc			\
	YQPkgUpdateProblemFilterView.cc		\
	YQPkgVersionsView.cc


BUILT_SOURCES =					\
	YQPackageSelector.moc			\
	YQPackageSelectorBase.moc		\
	YQPatternSelector.moc			\
	YQSimplePatchSelector.moc		\
	YQPkgChangeLogView.moc			\
	YQPkgChangesDialog.moc			\
	YQPkgConflictDialog.moc			\
	YQPkgConflictList.moc			\
	YQPkgDependenciesView.moc		\
	YQPkgDescriptionDialog.moc		\
	YQPkgDescriptionView.moc		\
	YQPkgDiskUsageList.moc			\
	YQPkgDiskUsageWarningDialog.moc		\
	YQPkgFileListView.moc			\
	YQPkgGenericDetailsView.moc		\
	YQPkgInstSrcFilterView.moc		\
	YQPkgInstSrcList.moc			\
	YQPkgLangList.moc			\
	YQPkgList.moc				\
	YQPkgObjList.moc			\
	YQPkgPatchFilterView.moc		\
	YQPkgPatchList.moc			\
	YQPkgPatternList.moc			\
	YQPkgProductDialog.moc			\
	YQPkgProductDialog.moc			\
	YQPkgProductList.moc			\
	YQPkgRpmGroupTagsFilterView.moc		\
	YQPkgSearchFilterView.moc		\
	YQPkgSelDescriptionView.moc		\
	YQPkgSelList.moc			\
	YQPkgStatusFilterView.moc		\
	YQPkgTechnicalDetailsView.moc		\
	YQPkgTextDialog.moc			\
	YQPkgUpdateProblemFilterView.moc	\
	YQPkgVersionsView.moc


YQPackageSelector.o YQPackageSelector.lo:			YQPackageSelector.moc
YQPackageSelectorBase.o YQPackageSelectorBase.lo:		YQPackageSelectorBase.moc
YQPatternSelector.o YQPatternSelector.lo:			YQPatternSelector.moc
YQSimplePatchSelector.o YQSimplePatchSelector.lo:		YQSimplePatchSelector.moc

YQPkgChangeLogView.o YQPkgChangeLogView.lo:			YQPkgChangeLogView.moc
YQPkgChangesDialog.o YQPkgChangesDialog.lo:			YQPkgChangesDialog.moc
YQPkgConflictDialog.o YQPkgConflictDialog.lo:			YQPkgConflictDialog.moc
YQPkgConflictList.o YQPkgConflictList.lo:			YQPkgConflictList.moc
YQPkgDependenciesView.o YQPkgDependenciesView.lo:		YQPkgDependenciesView.moc
YQPkgDescriptionDialog.o YQPkgDescriptionDialog.lo:		YQPkgDescriptionDialog.moc
YQPkgDescriptionView.o YQPkgDescriptionView.lo:			YQPkgDescriptionView.moc
YQPkgDiskUsageList.o YQPkgDiskUsageList.lo:			YQPkgDiskUsageList.moc
YQPkgDiskUsageWarningDialog.o YQPkgDiskUsageWarningDialog.lo:	YQPkgDiskUsageWarningDialog.moc
YQPkgFileListView.o YQPkgFileListView.lo:			YQPkgFileListView.moc
YQPkgGenericDetailsView.o YQPkgGenericDetailsView.lo:		YQPkgGenericDetailsView.moc
YQPkgInstSrcFilterView.o YQPkgInstSrcFilterView.lo:		YQPkgInstSrcFilterView.moc
YQPkgInstSrcList.o YQPkgInstSrcList.lo:				YQPkgInstSrcList.moc
YQPkgLangList.o YQPkgLangList.lo:				YQPkgLangList.moc
YQPkgList.o YQPkgList.lo:					YQPkgList.moc
YQPkgObjList.o YQPkgObjList.lo:					YQPkgObjList.moc
YQPkgPatchFilterView.o YQPkgPatchFilterView.lo:			YQPkgPatchFilterView.moc
YQPkgPatchList.o YQPkgPatchList.lo:				YQPkgPatchList.moc
YQPkgPatternList.o YQPkgPatternList.lo:				YQPkgPatternList.moc
YQPkgProductDialog.o YQPkgProductDialog.lo:			YQPkgProductDialog.moc
YQPkgProductList.o YQPkgProductList.lo:				YQPkgProductList.moc
YQPkgRpmGroupTagsFilterView.o YQPkgRpmGroupTagsFilterView.lo:	YQPkgRpmGroupTagsFilterView.moc
YQPkgSearchFilterView.o YQPkgSearchFilterView.lo:		YQPkgSearchFilterView.moc
YQPkgSelDescriptionView.o YQPkgSelDescriptionView.lo:		YQPkgSelDescriptionView.moc
YQPkgSelList.o YQPkgSelList.lo:					YQPkgSelList.moc
YQPkgStatusFilterView.o YQPkgStatusFilterView.lo:		YQPkgStatusFilterView.moc
YQPkgTechnicalDetailsView.o YQPkgTechnicalDetailsView.lo:	YQPkgTechnicalDetailsView.moc
YQPkgTextDialog.o YQPkgTextDialog.lo:				YQPkgTextDialog.moc
YQPkgUpdateProblemFilterView.o YQPkgUpdateProblemFilterView.lo:	YQPkgUpdateProblemFilterView.moc
YQPkgVersionsView.o YQPkgVersionsView.lo:			YQPkgVersionsView.moc


YQPackageSelector.moc:			YQPackageSelector.h
YQPackageSelectorBase.moc:		YQPackageSelectorBase.h
YQPatternSelector.moc:			YQPatternSelector.h
YQSimplePatchSelector.moc:		YQSimplePatchSelector.h

YQPkgChangeLogView.moc:			YQPkgChangeLogView.h
YQPkgChangesDialog.moc:			YQPkgChangesDialog.h
YQPkgConflictDialog.moc:		YQPkgConflictDialog.h
YQPkgConflictList.moc:			YQPkgConflictList.h
YQPkgDependenciesView.moc:		YQPkgDependenciesView.h
YQPkgDescriptionDialog.moc:		YQPkgDescriptionDialog.h
YQPkgDescriptionView.moc:		YQPkgDescriptionView.h
YQPkgDiskUsageList.moc:			YQPkgDiskUsageList.h
YQPkgDiskUsageWarningDialog.moc:	YQPkgDiskUsageWarningDialog.h
YQPkgFileListView.moc:			YQPkgFileListView.h
YQPkgGenericDetailsView.moc:		YQPkgGenericDetailsView.h
YQPkgInstSrcFilterView.moc:		YQPkgInstSrcFilterView.h
YQPkgInstSrcList.moc:			YQPkgInstSrcList.h
YQPkgLangList.moc:			YQPkgLangList.h
YQPkgList.moc:				YQPkgList.h
YQPkgObjList.moc:			YQPkgObjList.h
YQPkgPatchFilterView.moc:		YQPkgPatchFilterView.h
YQPkgPatchList.moc:			YQPkgPatchList.h
YQPkgPatternList.moc:			YQPkgPatternList.h
YQPkgProductDialog.moc:			YQPkgProductDialog.h
YQPkgProductList.moc:			YQPkgProductList.h
YQPkgRpmGroupTagsFilterView.moc:	YQPkgRpmGroupTagsFilterView.h
YQPkgSearchFilterView.moc:		YQPkgSearchFilterView.h
YQPkgSelDescriptionView.moc:		YQPkgSelDescriptionView.h
YQPkgSelList.moc:			YQPkgSelList.h
YQPkgStatusFilterView.moc:		YQPkgStatusFilterView.h
YQPkgTechnicalDetailsView.moc:		YQPkgTechnicalDetailsView.h
YQPkgTextDialog.moc:			YQPkgTextDialog.h
YQPkgUpdateProblemFilterView.moc:	YQPkgUpdateProblemFilterView.h
YQPkgVersionsView.moc:			YQPkgVersionsView.h


%.moc: %.h
	$(MOC) $< -o $@

CLEANFILES = *.moc

INCLUDES = -I$(srcdir)/.. -I$(QTDIR)/include -I$(includedir) -I$(includedir)/yui ${ZYPP_CFLAGS}
