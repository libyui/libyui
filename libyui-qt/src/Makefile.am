#
# Makefile.am for qt/src
#

SUBDIRS = include

AM_CXXFLAGS = -DY2LOG=\"ui-qt\"				\
	-DQT_THREAD_SUPPORT				\
	-DLOCALEDIR=\"${localedir}\"			\
	-DQT_LOCALEDIR=\"$(QTDIR)/translations\"	\
	-DICONDIR=\"${themedir}/current\"		\
	-DTHEMEDIR=\"${themedir}/current\"		\
	-DHELPIMAGEDIR=\"${imagedir}\"

MOC = $(QTDIR)/bin/moc

serversdir = $(execcompdir)/servers

# plugin, libtool forces 'lib' prefix
plugin_LTLIBRARIES = libpy2qt.la
noinst_LTLIBRARIES = liby2qt.la
libpy2qt_la_LDFLAGS = -version-info 2:0
liby2qt_la_LDFLAGS = -version-info 2:0

libpy2qt_la_LIBADD =			\
	-lyui				\
	-L$(QTLIBDIR) -lqt-mt		\
	-lX11 -lXext -lSM -lICE		\
	-ly2pm

liby2qt_la_LIBADD = $(libpy2qt_la_LIBADD)

# Make library, component plugin, and linkable object

# the basic library has the pure y2qt sources

liby2qt_la_SOURCES = 				\
	YQUI_core.cc				\
	YQUI_widgets.cc				\
	YQUI_builtins.cc			\
	YQUI_x11.cc				\
						\
	YQIconPool.cc				\
						\
	YQAlignment.cc				\
	YQBarGraph.cc				\
	YQCheckBox.cc				\
	YQColoredLabel.cc			\
	YQComboBox.cc				\
	YQDate.cc				\
	YQDialog.cc				\
	YQDownloadProgress.cc			\
	YQDumbTab.cc				\
	YQEBunny.cc				\
	YQEmpty.cc				\
	YQFrame.cc				\
	YQGenericButton.cc			\
	YQImage.cc				\
	YQIntField.cc				\
	YQLabel.cc				\
	YQLogView.cc				\
	YQMenuButton.cc				\
	YQMultiLineEdit.cc			\
	YQMultiProgressMeter.cc			\
	YQMultiSelectionBox.cc			\
	YQPackageSelector.cc			\
	YQPackageSelectorHelp.cc		\
	YQPartitionSplitter.cc			\
	YQPkgChangesDialog.cc			\
	YQPkgConflictDialog.cc			\
	YQPkgConflictList.cc			\
	YQPkgDependenciesView.cc		\
	YQPkgDescriptionDialog.cc		\
	YQPkgDescriptionView.cc			\
	YQPkgDiskUsageList.cc			\
	YQPkgDiskUsageWarningDialog.cc		\
	YQPkgGenericDetailsView.cc		\
	YQPkgLangFilterView.cc			\
	YQPkgLangSelList.cc			\
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
	YQTime.cc				\
	YQTree.cc				\
	YQWizard.cc				\
	YQWizardButton.cc			\
						\
	YFrameBuffer.cc				\
	QY2BarGraph.cc				\
	QY2ComboTabWidget.cc			\
	QY2DiskUsageList.cc			\
	QY2FloppyMountDialog.cc			\
	QY2CharValidator.cc			\
	QY2ListView.cc				\
	QY2LayoutUtils.cc			\
	qxembed.cc

# Note: QY2LayoutUtils doesn't need any .moc file: No QObject, just functions.


# the plugin adds the liby2 component interface

libpy2qt_la_SOURCES =				\
	$(liby2qt_la_SOURCES)			\
	Y2CCQt.cc


BUILT_SOURCES =					\
	YQUI.moc				\
						\
	YQAlignment.moc				\
	YQBarGraph.moc				\
	YQCheckBox.moc				\
	YQColoredLabel.moc			\
	YQComboBox.moc				\
	YQDialog.moc				\
	YQDownloadProgress.moc			\
	YQDumbTab.moc				\
	YQEmpty.moc				\
	YQFrame.moc				\
	YQGenericButton.moc			\
	YQImage.moc				\
	YQIntField.moc				\
	YQLabel.moc				\
	YQLogView.moc				\
	YQMenuButton.moc			\
	YQMultiLineEdit.moc			\
	YQMultiProgressMeter.moc		\
	YQMultiSelectionBox.moc			\
	YQPackageSelector.moc			\
	YQPartitionSplitter.moc			\
	YQPkgChangesDialog.moc			\
	YQPkgConflictDialog.moc			\
	YQPkgConflictList.moc			\
	YQPkgDependenciesView.moc		\
	YQPkgDescriptionDialog.moc		\
	YQPkgDescriptionView.moc		\
	YQPkgDiskUsageList.moc			\
	YQPkgDiskUsageWarningDialog.moc		\
	YQPkgGenericDetailsView.moc		\
	YQPkgLangFilterView.moc			\
	YQPkgLangSelList.moc			\
	YQPkgList.moc				\
	YQPkgObjList.moc			\
	YQPkgRpmGroupTagsFilterView.moc		\
	YQPkgSearchFilterView.moc		\
	YQPkgSelList.moc			\
	YQPkgSelectionsFilterView.moc		\
	YQPkgStatusFilterView.moc		\
	YQPkgTechnicalDetailsView.moc		\
	YQPkgTextDialog.moc			\
	YQPkgUpdateProblemFilterView.moc	\
	YQPkgVersionsView.moc			\
	YQPkgYouPatchFilterView.moc		\
	YQPkgYouPatchList.moc			\
	YQProgressBar.moc			\
	YQPushButton.moc			\
	YQRadioButton.moc			\
	YQRadioButtonGroup.moc			\
	YQReplacePoint.moc			\
	YQRichText.moc				\
	YQSelectionBox.moc			\
	YQSlider.moc				\
	YQSpacing.moc				\
	YQSplit.moc				\
	YQSquash.moc				\
	YQTable.moc				\
	YQTextEntry.moc				\
	YQTree.moc				\
	YQWizard.moc				\
	YQWizardButton.moc			\
						\
	QY2BarGraph.moc				\
	QY2ComboTabWidget.moc			\
	QY2DiskUsageList.moc			\
	QY2FloppyMountDialog.moc		\
	QY2CharValidator.moc			\
	QY2ListView.moc				\
	qxembed.moc


YQUI_core.o YQUI_core.lo:					YQUI.moc
YQUI_widgets.o YQUI_widgets.lo:					YQUI.moc
YQUI_builtins.o YQUI_builtins.lo:				YQUI.moc
YQUI_x11.o YQUI_x11.lo:						YQUI.moc

YQAlignment.o YQAlignment.lo:   				YQAlignment.moc
YQBarGraph.o YQBarGraph.lo:   					YQBarGraph.moc
YQCheckBox.o YQCheckBox.lo:					YQCheckBox.moc
YQColoredLabel.o YQColoredLabel.lo:				YQColoredLabel.moc
YQComboBox.o YQComboBox.lo:					YQComboBox.moc
YQDate.o YQDate.lo:						YQDate.moc
YQDialog.o YQDialog.lo:						YQDialog.moc
YQDownloadProgress.o YQDownloadProgress.lo:			YQDownloadProgress.moc
YQDumbTab.o YQDumbTab.lo:					YQDumbTab.moc
YQEmpty.o YQEmpty.lo:						YQEmpty.moc
YQFrame.o YQFrame.lo:						YQFrame.moc
YQGenericButton.o YQGenericButton.lo:				YQGenericButton.moc
YQImage.o YQImage.lo:						YQImage.moc
YQIntField.o YQIntField.lo:					YQIntField.moc
YQLabel.o YQLabel.lo:						YQLabel.moc
YQLogView.o YQLogView.lo:					YQLogView.moc
YQMenuButton.o YQMenuButton.lo:					YQMenuButton.moc
YQMultiLineEdit.o YQMultiLineEdit.lo:				YQMultiLineEdit.moc
YQMultiProgressMeter.o YQMultiProgressMeter.lo:			YQMultiProgressMeter.moc
YQMultiSelectionBox.o YQMultiSelectionBox.lo:			YQMultiSelectionBox.moc
YQPackageSelector.o YQPackageSelector.lo:			YQPackageSelector.moc
YQPartitionSplitter.o YQPartitionSplitter.lo:			YQPartitionSplitter.moc
YQPkgChangesDialog.o YQPkgChangesDialog.lo:			YQPkgChangesDialog.moc
YQPkgConflictDialog.o YQPkgConflictDialog.lo:			YQPkgConflictDialog.moc
YQPkgConflictList.o YQPkgConflictList.lo:			YQPkgConflictList.moc
YQPkgDependenciesView.o YQPkgDependenciesView.lo:		YQPkgDependenciesView.moc
YQPkgDescriptionDialog.o YQPkgDescriptionDialog.lo:		YQPkgDescriptionDialog.moc
YQPkgDescriptionView.o YQPkgDescriptionView.lo:			YQPkgDescriptionView.moc
YQPkgDiskUsageList.o YQPkgDiskUsageList.lo:			YQPkgDiskUsageList.moc
YQPkgDiskUsageWarningDialog.o YQPkgDiskUsageWarningDialog.lo:	YQPkgDiskUsageWarningDialog.moc
YQPkgGenericDetailsView.o YQPkgGenericDetailsView.lo:		YQPkgGenericDetailsView.moc
YQPkgLangFilterView.o YQPkgLangFilterView.lo:			YQPkgLangFilterView.moc
YQPkgLangSelList.o YQPkgLangSelList.lo:				YQPkgLangSelList.moc
YQPkgList.o YQPkgList.lo:					YQPkgList.moc
YQPkgObjList.o YQPkgObjList.lo:					YQPkgObjList.moc
YQPkgRpmGroupTagsFilterView.o YQPkgRpmGroupTagsFilterView.lo:	YQPkgRpmGroupTagsFilterView.moc
YQPkgSearchFilterView.o YQPkgSearchFilterView.lo:		YQPkgSearchFilterView.moc
YQPkgSelList.o YQPkgSelList.lo:					YQPkgSelList.moc
YQPkgSelectionsFilterView.o YQPkgSelectionsFilterView.lo:	YQPkgSelectionsFilterView.moc
YQPkgStatusFilterView.o YQPkgStatusFilterView.lo:		YQPkgStatusFilterView.moc
YQPkgTechnicalDetailsView.o YQPkgTechnicalDetailsView.lo:	YQPkgTechnicalDetailsView.moc
YQPkgTextDialog.o YQPkgTextDialog.lo:				YQPkgTextDialog.moc
YQPkgUpdateProblemFilterView.o YQPkgUpdateProblemFilterView.lo:	YQPkgUpdateProblemFilterView.moc
YQPkgVersionsView.o YQPkgVersionsView.lo:			YQPkgVersionsView.moc
YQPkgYouPatchFilterView.o YQPkgYouPatchFilterView.lo:		YQPkgYouPatchFilterView.moc
YQPkgYouPatchList.o YQPkgYouPatchList.lo:			YQPkgYouPatchList.moc
YQProgressBar.o YQProgressBar.lo:				YQProgressBar.moc
YQPushButton.o YQPushButton.lo:					YQPushButton.moc
YQRadioButton.o YQRadioButton.lo:				YQRadioButton.moc
YQRadioButtonGroup.o YQRadioButtonGroup.lo:			YQRadioButtonGroup.moc
YQReplacePoint.o YQReplacePoint.lo:				YQReplacePoint.moc
YQRichText.o YQRichText.lo:					YQRichText.moc
YQSelectionBox.o YQSelectionBox.lo:				YQSelectionBox.moc
YQSlider.o YQSlider.lo:						YQSlider.moc
YQSpacing.o YQSpacing.lo:					YQSpacing.moc
YQSplit.o YQSplit.lo:						YQSplit.moc
YQSquash.o YQSquash.lo:						YQSquash.moc
YQTable.o YQTable.lo:						YQTable.moc
YQTextEntry.o YQTextEntry.lo:					YQTextEntry.moc
YQTime.o YQTime.lo:						YQTime.moc
YQTree.o YQTree.lo:						YQTree.moc
YQWizard.o YQWizard.lo:						YQWizard.moc
YQWizardButton.o YQWizardButton.lo:				YQWizardButton.moc

QY2BarGraph.o QY2BarGraph.lo:					QY2BarGraph.moc
QY2ComboTabWidget.o QY2ComboTabWidget.lo:			QY2ComboTabWidget.cc
QY2DiskUsageList.o QY2DiskUsageList.lo:				QY2DiskUsageList.cc
QY2FloppyMountDialog.o QY2FloppyMountDialog.lo:			QY2FloppyMountDialog.cc
QY2CharValidator.o QY2CharValidator.lo:				QY2CharValidator.moc
QY2ListView.o QY2ListView.lo:					QY2ListView.moc
qxembed.o qxembed.lo:						qxembed.moc



YQUI.moc:				include/YQUI.h

YQAlignment.moc:			include/YQAlignment.h
YQBarGraph.moc:				include/YQBarGraph.h
YQCheckBox.moc:				include/YQCheckBox.h
YQColoredLabel.moc:			include/YQColoredLabel.h
YQComboBox.moc:				include/YQComboBox.h
YQDialog.moc: 				include/YQDialog.h
YQDownloadProgress.moc:			include/YQDownloadProgress.h
YQDumbTab.moc:				include/YQDumbTab.h
YQEmpty.moc:				include/YQEmpty.h
YQFrame.moc:				include/YQFrame.h
YQGenericButton.moc:			include/YQGenericButton.h
YQImage.moc:				include/YQImage.h
YQIntField.moc:				include/YQIntField.h
YQLabel.moc:				include/YQLabel.h
YQLogView.moc:				include/YQLogView.h
YQMenuButton.moc:			include/YQMenuButton.h
YQMultiLineEdit.moc:			include/YQMultiLineEdit.h
YQMultiProgressMeter.moc:		include/YQMultiProgressMeter.h
YQMultiSelectionBox.moc:		include/YQMultiSelectionBox.h
YQPackageSelector.moc:			include/YQPackageSelector.h
YQPartitionSplitter.moc:		include/YQPartitionSplitter.h
YQPkgChangesDialog.moc:			include/YQPkgChangesDialog.h
YQPkgConflictDialog.moc:		include/YQPkgConflictDialog.h
YQPkgConflictList.moc:			include/YQPkgConflictList.h
YQPkgDependenciesView.moc:		include/YQPkgDependenciesView.h
YQPkgDescriptionDialog.moc:		include/YQPkgDescriptionDialog.h
YQPkgDescriptionView.moc:		include/YQPkgDescriptionView.h
YQPkgDiskUsageList.moc:			include/YQPkgDiskUsageList.h
YQPkgDiskUsageWarningDialog.moc:	include/YQPkgDiskUsageWarningDialog.h
YQPkgGenericDetailsView.moc:		include/YQPkgGenericDetailsView.h
YQPkgLangFilterView.moc:		include/YQPkgLangFilterView.h
YQPkgLangSelList.moc:			include/YQPkgLangSelList.h
YQPkgList.moc:				include/YQPkgList.h
YQPkgObjList.moc:			include/YQPkgObjList.h
YQPkgRpmGroupTagsFilterView.moc:	include/YQPkgRpmGroupTagsFilterView.h
YQPkgSearchFilterView.moc:		include/YQPkgSearchFilterView.h
YQPkgSelList.moc:			include/YQPkgSelList.h
YQPkgSelectionsFilterView.moc:		include/YQPkgSelectionsFilterView.h
YQPkgStatusFilterView.moc:		include/YQPkgStatusFilterView.h
YQPkgTechnicalDetailsView.moc:		include/YQPkgTechnicalDetailsView.h
YQPkgTextDialog.moc:			include/YQPkgTextDialog.h
YQPkgUpdateProblemFilterView.moc:	include/YQPkgUpdateProblemFilterView.h
YQPkgVersionsView.moc:			include/YQPkgVersionsView.h
YQPkgYouPatchFilterView.moc:		include/YQPkgYouPatchFilterView.h
YQPkgYouPatchList.moc:			include/YQPkgYouPatchList.h
YQProgressBar.moc:			include/YQProgressBar.h
YQPushButton.moc:			include/YQPushButton.h
YQRadioButton.moc:			include/YQRadioButton.h
YQRadioButtonGroup.moc:      		include/YQRadioButtonGroup.h
YQReplacePoint.moc:			include/YQReplacePoint.h
YQRichText.moc:				include/YQRichText.h
YQSelectionBox.moc:			include/YQSelectionBox.h
YQSlider.moc:				include/YQSlider.h
YQSpacing.moc:				include/YQSpacing.h
YQSplit.moc:				include/YQSplit.h
YQSquash.moc:				include/YQSquash.h
YQTable.moc:				include/YQTable.h
YQTextEntry.moc:			include/YQTextEntry.h
YQTree.moc:				include/YQTree.h
YQWizard.moc:				include/YQWizard.h
YQWizardButton.moc:			include/YQWizardButton.h

QY2BarGraph.moc:			include/QY2BarGraph.h
QY2ComboTabWidget.cc:			include/QY2ComboTabWidget.h
QY2DiskUsageList.cc:			include/QY2DiskUsageList.h
QY2FloppyMountDialog.cc:		include/QY2FloppyMountDialog.h
QY2CharValidator.moc:			include/QY2CharValidator.h
QY2ListView.moc:			include/QY2ListView.h

qxembed.moc:				include/qxembed.h


%.moc: include/%.h
	$(MOC) $< -o $@

CLEANFILES = *.moc y2qt.kdoc y2ui.kdoc

INCLUDES = -I$(srcdir)/include -I$(QTDIR)/include -I$(includedir) -I$(includedir)/yui
