#
# Makefile.am for qt/src
#

SUBDIRS = include

AM_CXXFLAGS = -DY2LOG=\"ui-qt\" -DQT_THREAD_SUPPORT -DLOCALEDIR=\"${localedir}\" -DICONDIR=\"${themedir}/current\"

MOC = $(QTDIR)/bin/moc

serversdir = $(execcompdir)/servers
servers_PROGRAMS = qt

# plugin, libtool forces 'lib' prefix
plugin_LTLIBRARIES = libpy2qt.la
noinst_LTLIBRARIES = liby2qt.la
libpy2qt_la_LDFLAGS = -version-info 2:0
liby2qt_la_LDFLAGS = -version-info 2:0

libpy2qt_la_LIBADD =			\
	-L$(QTLIBDIR) -lqt-mt		\
	-lX11 -lXext -lSM -lICE		\
	-ly2pm

liby2qt_la_LIBADD = $(libpy2qt_la_LIBADD)

# Make library, component plugin, and linkable object

# the basic library has the pure y2qt sources

liby2qt_la_SOURCES = 				\
	YUIQt.cc				\
	YQIconPool.cc				\
	layoututils.cc				\
						\
	YQAlignment.cc				\
	YQBarGraph.cc				\
	YQCheckBox.cc				\
	YQColoredLabel.cc			\
	YQComboBox.cc				\
	YQDialog.cc				\
	YQDownloadProgress.cc			\
	YQEmpty.cc				\
	YQFrame.cc				\
	YQImage.cc				\
	YQIntField.cc				\
	YQLabel.cc				\
	YQLogView.cc				\
	YQMenuButton.cc				\
	YQMultiLineEdit.cc			\
	YQMultiSelectionBox.cc			\
	YQPackageSelector.cc			\
	YQPartitionSplitter.cc			\
	YQPkgConflictDialog.cc			\
	YQPkgConflictList.cc			\
	YQPkgDescriptionView.cc			\
	YQPkgDependenciesView.cc		\
	YQPkgGenericDetailsView.cc		\
	YQPkgList.cc				\
	YQPkgObjList.cc				\
	YQPkgRpmGroupTagsFilterView.cc		\
	YQPkgSearchFilterView.cc		\
	YQPkgSelList.cc				\
	YQPkgSelectionsFilterView.cc		\
	YQPkgStatusFilterView.cc		\
	YQPkgTechnicalDetailsView.cc		\
	YQPkgTextDialog.cc			\
	YQPkgUpdateProblemFilterView.cc		\
	YQPkgVersionsView.cc			\
	YQPkgYouPatchFilterView.cc		\
	YQPkgYouPatchList.cc			\
	YQProgressBar.cc			\
	YQPushButton.cc				\
	YQRadioButton.cc			\
	YQRadioButtonGroup.cc			\
	YQReplacePoint.cc			\
	YQRichText.cc				\
	YQSelectionBox.cc			\
	YQSlider.cc				\
	YQSpacing.cc				\
	YQSplit.cc				\
	YQSquash.cc				\
	YQTable.cc				\
	YQTextEntry.cc				\
	YQTree.cc				\
						\
	QY2BarGraph.cc				\
	QY2ComboTabWidget.cc			\
	QY2DiskUsageList.cc			\
	QY2CharValidator.cc			\
	QY2ListView.cc				\
	QXEmbed.cc

# the plugin adds the liby2 component interface

libpy2qt_la_SOURCES =				\
	$(liby2qt_la_SOURCES)			\
	Y2CCQt.cc				\
	Y2QtComponent.cc

# now the stand alone qt version

qt_SOURCES =

qt_LDADD = libpy2qt.la -lycp -ly2		\
	-lyui -lpthread -lz			\
	-lutil

qt_LDFLAGS =					\
	-Xlinker --whole-archive		\
	$(plugindir)/libpy2plugin.la		\
	$(plugindir)/libpy2stdio.la		\
	-Xlinker --no-whole-archive

BUILT_SOURCES =					\
	YUIQt.moc.cc				\
						\
	YQAlignment.moc.cc			\
	YQBarGraph.moc.cc			\
	YQCheckBox.moc.cc			\
	YQColoredLabel.moc.cc			\
	YQComboBox.moc.cc			\
	YQDialog.moc.cc				\
	YQDownloadProgress.moc.cc		\
	YQEmpty.moc.cc				\
	YQFrame.moc.cc				\
	YQImage.moc.cc				\
	YQIntField.moc.cc			\
	YQLabel.moc.cc				\
	YQLogView.moc.cc			\
	YQMenuButton.moc.cc			\
	YQMultiLineEdit.moc.cc			\
	YQMultiSelectionBox.moc.cc		\
	YQPackageSelector.moc.cc		\
	YQPartitionSplitter.moc.cc		\
	YQPkgConflictDialog.moc.cc		\
	YQPkgConflictList.moc.cc		\
	YQPkgDescriptionView.moc.cc		\
	YQPkgDependenciesView.moc.cc		\
	YQPkgGenericDetailsView.moc.cc		\
	YQPkgList.moc.cc			\
	YQPkgObjList.moc.cc			\
	YQPkgRpmGroupTagsFilterView.moc.cc	\
	YQPkgSearchFilterView.moc.cc		\
	YQPkgSelList.moc.cc			\
	YQPkgSelectionsFilterView.moc.cc	\
	YQPkgStatusFilterView.moc.cc		\
	YQPkgTechnicalDetailsView.moc.cc	\
	YQPkgTextDialog.moc.cc			\
	YQPkgUpdateProblemFilterView.moc.cc	\
	YQPkgVersionsView.moc.cc		\
	YQPkgYouPatchFilterView.moc.cc		\
	YQPkgYouPatchList.moc.cc		\
	YQProgressBar.moc.cc			\
	YQPushButton.moc.cc			\
	YQRadioButton.moc.cc			\
	YQRadioButtonGroup.moc.cc		\
	YQReplacePoint.moc.cc			\
	YQRichText.moc.cc			\
	YQSelectionBox.moc.cc			\
	YQSlider.moc.cc				\
	YQSpacing.moc.cc			\
	YQSplit.moc.cc				\
	YQSquash.moc.cc				\
	YQTable.moc.cc				\
	YQTextEntry.moc.cc			\
	YQTree.moc.cc				\
						\
	QY2BarGraph.moc.cc			\
	QY2ComboTabWidget.moc.cc		\
	QY2DiskUsageList.moc.cc			\
	QY2CharValidator.moc.cc			\
	QY2ListView.moc.cc			\
	QXEmbed.moc.cc


YUIQt.o YUIQt.lo:					YUIQt.moc.cc

YQAlignment.o YQAlignment.lo:   				YQAlignment.moc.cc
YQBarGraph.o YQBarGraph.lo:   					YQBarGraph.moc.cc
YQCheckBox.o YQCheckBox.lo:					YQCheckBox.moc.cc
YQColoredLabel.o YQColoredLabel.lo:				YQColoredLabel.moc.cc
YQComboBox.o YQComboBox.lo:					YQComboBox.moc.cc
YQDialog.o YQDialog.lo:						YQDialog.moc.cc
YQDownloadProgress.o YQDownloadProgress.lo:			YQDownloadProgress.moc.cc
YQEmpty.o YQEmpty.lo:						YQEmpty.moc.cc
YQFrame.o YQFrame.lo:						YQFrame.moc.cc
YQImage.o YQImage.lo:						YQImage.moc.cc
YQIntField.o YQIntField.lo:					YQIntField.moc.cc
YQLabel.o YQLabel.lo:						YQLabel.moc.cc
YQLogView.o YQLogView.lo:					YQLogView.moc.cc
YQMenuButton.o YQMenuButton.lo:					YQMenuButton.moc.cc
YQMultiLineEdit.o YQMultiLineEdit.lo:				YQMultiLineEdit.moc.cc
YQMultiSelectionBox.o YQMultiSelectionBox.lo:			YQMultiSelectionBox.moc.cc
YQPackageSelector.o YQPackageSelector.lo:			YQPackageSelector.moc.cc
YQPartitionSplitter.o YQPartitionSplitter.lo:			YQPartitionSplitter.moc.cc
YQPkgConflictDialog.o YQPkgConflictDialog.lo:			YQPkgConflictDialog.moc.cc
YQPkgConflictList.o YQPkgConflictList.lo:			YQPkgConflictList.moc.cc
YQPkgDescriptionView.o YQPkgDescriptionView.lo:			YQPkgDescriptionView.moc.cc
YQPkgDependenciesView.o YQPkgDependenciesView.lo:		YQPkgDependenciesView.moc.cc
YQPkgGenericDetailsView.o YQPkgGenericDetailsView.lo:		YQPkgGenericDetailsView.moc.cc
YQPkgList.o YQPkgList.lo:					YQPkgList.moc.cc
YQPkgObjList.o YQPkgObjList.lo:					YQPkgObjList.moc.cc
YQPkgRpmGroupTagsFilterView.o YQPkgRpmGroupTagsFilterView.lo:	YQPkgRpmGroupTagsFilterView.moc.cc
YQPkgSearchFilterView.o YQPkgSearchFilterView.lo:		YQPkgSearchFilterView.moc.cc
YQPkgSelList.o YQPkgSelList.lo:					YQPkgSelList.moc.cc
YQPkgSelectionsFilterView.o YQPkgSelectionsFilterView.lo:	YQPkgSelectionsFilterView.moc.cc
YQPkgStatusFilterView.o YQPkgStatusFilterView.lo:		YQPkgStatusFilterView.moc.cc
YQPkgTechnicalDetailsView.o YQPkgTechnicalDetailsView.lo:	YQPkgTechnicalDetailsView.moc.cc
YQPkgTextDialog.o YQPkgTextDialog.lo:				YQPkgTextDialog.moc.cc
YQPkgUpdateProblemFilterView.o YQPkgUpdateProblemFilterView.lo:	YQPkgUpdateProblemFilterView.moc.cc
YQPkgVersionsView.o YQPkgVersionsView.lo:			YQPkgVersionsView.moc.cc
YQPkgYouPatchFilterView.o YQPkgYouPatchFilterView.lo:		YQPkgYouPatchFilterView.moc.cc
YQPkgYouPatchList.o YQPkgYouPatchList.lo:			YQPkgYouPatchList.moc.cc
YQProgressBar.o YQProgressBar.lo:				YQProgressBar.moc.cc
YQPushButton.o YQPushButton.lo:					YQPushButton.moc.cc
YQRadioButton.o YQRadioButton.lo:				YQRadioButton.moc.cc
YQRadioButtonGroup.o YQRadioButtonGroup.lo:			YQRadioButtonGroup.moc.cc
YQReplacePoint.o YQReplacePoint.lo:				YQReplacePoint.moc.cc
YQRichText.o YQRichText.lo:					YQRichText.moc.cc
YQSelectionBox.o YQSelectionBox.lo:				YQSelectionBox.moc.cc
YQSlider.o YQSlider.lo:						YQSlider.moc.cc
YQSpacing.o YQSpacing.lo:					YQSpacing.moc.cc
YQSplit.o YQSplit.lo:						YQSplit.moc.cc
YQSquash.o YQSquash.lo:						YQSquash.moc.cc
YQTable.o YQTable.lo:						YQTable.moc.cc
YQTextEntry.o YQTextEntry.lo:					YQTextEntry.moc.cc
YQTree.o YQTree.lo:						YQTree.moc.cc

QY2BarGraph.o QY2BarGraph.lo:					QY2BarGraph.moc.cc
QY2ComboTabWidget.o QY2ComboTabWidget.lo:			QY2ComboTabWidget.cc
QY2DiskUsageList.o QY2DiskUsageList.lo:				QY2DiskUsageList.cc
QY2CharValidator.o QY2CharValidator.lo:				QY2CharValidator.moc.cc
QY2ListView.o QY2ListView.lo:					QY2ListView.moc.cc
QXEmbed.o QXEmbed.lo:						QXEmbed.moc.cc



YUIQt.moc.cc:				include/YUIQt.h

YQAlignment.moc.cc:			include/YQAlignment.h
YQBarGraph.moc.cc:			include/YQBarGraph.h
YQCheckBox.moc.cc:			include/YQCheckBox.h
YQColoredLabel.moc.cc:			include/YQColoredLabel.h
YQComboBox.moc.cc:			include/YQComboBox.h
YQDialog.moc.cc: 			include/YQDialog.h
YQDownloadProgress.moc.cc:		include/YQDownloadProgress.h
YQEmpty.moc.cc:				include/YQEmpty.h
YQFrame.moc.cc:				include/YQFrame.h
YQImage.moc.cc:				include/YQImage.h
YQIntField.moc.cc:			include/YQIntField.h
YQLabel.moc.cc:				include/YQLabel.h
YQLogView.moc.cc:			include/YQLogView.h
YQMenuButton.moc.cc:			include/YQMenuButton.h
YQMultiLineEdit.moc.cc:			include/YQMultiLineEdit.h
YQMultiSelectionBox.moc.cc:		include/YQMultiSelectionBox.h
YQPackageSelector.moc.cc:		include/YQPackageSelector.h
YQPartitionSplitter.moc.cc:		include/YQPartitionSplitter.h
YQPkgConflictDialog.moc.cc:		include/YQPkgConflictDialog.h
YQPkgConflictList.moc.cc:		include/YQPkgConflictList.h
YQPkgDescriptionView.moc.cc:		include/YQPkgDescriptionView.h
YQPkgDependenciesView.moc.cc:		include/YQPkgDependenciesView.h
YQPkgGenericDetailsView.moc.cc:		include/YQPkgGenericDetailsView.h
YQPkgList.moc.cc:			include/YQPkgList.h
YQPkgObjList.moc.cc:			include/YQPkgObjList.h
YQPkgRpmGroupTagsFilterView.moc.cc:	include/YQPkgRpmGroupTagsFilterView.h
YQPkgSearchFilterView.moc.cc:		include/YQPkgSearchFilterView.h
YQPkgSelList.moc.cc:			include/YQPkgSelList.h
YQPkgSelectionsFilterView.moc.cc:	include/YQPkgSelectionsFilterView.h
YQPkgStatusFilterView.moc.cc:		include/YQPkgStatusFilterView.h
YQPkgTechnicalDetailsView.moc.cc:	include/YQPkgTechnicalDetailsView.h
YQPkgTextDialog.moc.cc:			include/YQPkgTextDialog.h
YQPkgUpdateProblemFilterView.moc.cc:	include/YQPkgUpdateProblemFilterView.h
YQPkgVersionsView.moc.cc:		include/YQPkgVersionsView.h
YQPkgYouPatchFilterView.moc.cc:		include/YQPkgYouPatchFilterView.h
YQPkgYouPatchList.moc.cc:		include/YQPkgYouPatchList.h
YQProgressBar.moc.cc:			include/YQProgressBar.h
YQPushButton.moc.cc:			include/YQPushButton.h
YQRadioButton.moc.cc:			include/YQRadioButton.h
YQRadioButtonGroup.moc.cc:      	include/YQRadioButtonGroup.h
YQReplacePoint.moc.cc:			include/YQReplacePoint.h
YQRichText.moc.cc:			include/YQRichText.h
YQSelectionBox.moc.cc:			include/YQSelectionBox.h
YQSlider.moc.cc:			include/YQSlider.h
YQSpacing.moc.cc:			include/YQSpacing.h
YQSplit.moc.cc:				include/YQSplit.h
YQSquash.moc.cc:			include/YQSquash.h
YQTable.moc.cc:				include/YQTable.h
YQTextEntry.moc.cc:			include/YQTextEntry.h
YQTree.moc.cc:				include/YQTree.h

QY2BarGraph.moc.cc:			include/QY2BarGraph.h
QY2ComboTabWidget.cc:			include/QY2ComboTabWidget.h
QY2DiskUsageList.cc:			include/QY2DiskUsageList.h
QY2CharValidator.moc.cc:		include/QY2CharValidator.h
QY2ListView.moc.cc:			include/QY2ListView.h
QXEmbed.moc.cc:				include/QXEmbed.h


%.moc.cc: include/%.h
	$(MOC) $< -o $@

EXTRA_DIST = yast2.xpm

CLEANFILES = *.moc.cc y2qt.kdoc y2ui.kdoc

INCLUDES = -I$(srcdir)/include -I$(QTDIR)/include -I$(includedir) -I$(includedir)/yui
