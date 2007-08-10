#
# Makefile.am for qt/src
#

SUBDIRS = pkg

AM_CXXFLAGS = -DY2LOG=\"ui-qt\"					\
	-DQT_THREAD_SUPPORT					\
	-DLOCALEDIR=\"${localedir}\"				\
	-DQT_LOCALEDIR=\"$(QTDIR)/translations\"		\
	-DICONDIR=\"${themedir}/current\"			\
	-DTHEMEDIR=\"${themedir}/current\"			\
	-DHELPIMAGEDIR=\"${imagedir}\"				\
	-DLANG_FONTS_FILE=\"${lang_fontsdir}/lang_fonts\"	\
	-DQTLIBDIR=\"$(QTLIBDIR)\"				\
	-fmessage-length=0


MOC = $(QTDIR)/bin/moc

serversdir = $(execcompdir)/servers

lang_fontsdir=$(ydatadir)
lang_fonts_DATA = lang_fonts

EXTRA_DIST = $(lang_fonts_DATA)

# plugin, libtool forces 'lib' prefix
plugin_LTLIBRARIES = libpy2qt.la
libpy2qt_la_LDFLAGS = -version-info 2:0

libpy2qt_la_LIBADD =			\
	-L$(plugindir) -lpy2UI		\
	-L$(QTLIBDIR) -lqt-mt		\
	-lX11 -lXext -lSM -lICE

# libpy2qt_la_LIBADD =			\
# 	-Lpkg -ly2qt_pkg		\
# 	-L$(libdir) -lpy2UI		\
# 	-L$(QTLIBDIR) -lqt-mt		\
# 	-lX11 -lXext -lSM -lICE

# noinst_LTLIBRARIES = liby2qt.la
# liby2qt_la_LDFLAGS = -version-info 2:0
# liby2qt_la_LIBADD = $(libpy2qt_la_LIBADD)


noinst_HEADERS = $(wildcard *.h)

libpy2qt_la_SOURCES =				\
	Y2CCQt.cc				\
						\
	YQUI_core.cc				\
	YQUI_widgets.cc				\
	YQUI_builtins.cc			\
	YQUI_x11.cc				\
	YQPackageSelectorPlugin.cc		\
						\
	YQIconPool.cc				\
						\
	YQAlignment.cc				\
	YQBarGraph.cc				\
	YQCheckBox.cc				\
	YQCheckBoxFrame.cc			\
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
	YQPartitionSplitter.cc			\
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
	QY2Settings.cc				\
	QY2LayoutUtils.cc			\
	qxembed.cc




# Note: QY2LayoutUtils and QY2Settings don't need any .moc file: No QObjects


# the plugin adds the liby2 component interface
#
# libpy2qt_la_SOURCES =				\
#	$(liby2qt_la_SOURCES)			\
#	Y2CCQt.cc


BUILT_SOURCES =					\
	YQUI.moc				\
						\
	YQAlignment.moc				\
	YQBarGraph.moc				\
	YQCheckBox.moc				\
	YQCheckBoxFrame.moc			\
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
	YQPartitionSplitter.moc			\
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
YQCheckBoxFrame.o YQCheckBoxFrame.lo:				YQCheckBoxFrame.moc
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
YQPartitionSplitter.o YQPartitionSplitter.lo:			YQPartitionSplitter.moc
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



YQUI.moc:				YQUI.h

YQAlignment.moc:			YQAlignment.h
YQBarGraph.moc:				YQBarGraph.h
YQCheckBox.moc:				YQCheckBox.h
YQCheckBoxFrame.moc:			YQCheckBoxFrame.h
YQColoredLabel.moc:			YQColoredLabel.h
YQComboBox.moc:				YQComboBox.h
YQDialog.moc: 				YQDialog.h
YQDownloadProgress.moc:			YQDownloadProgress.h
YQDumbTab.moc:				YQDumbTab.h
YQEmpty.moc:				YQEmpty.h
YQFrame.moc:				YQFrame.h
YQGenericButton.moc:			YQGenericButton.h
YQImage.moc:				YQImage.h
YQIntField.moc:				YQIntField.h
YQLabel.moc:				YQLabel.h
YQLogView.moc:				YQLogView.h
YQMenuButton.moc:			YQMenuButton.h
YQMultiLineEdit.moc:			YQMultiLineEdit.h
YQMultiProgressMeter.moc:		YQMultiProgressMeter.h
YQMultiSelectionBox.moc:		YQMultiSelectionBox.h
YQPartitionSplitter.moc:		YQPartitionSplitter.h
YQProgressBar.moc:			YQProgressBar.h
YQPushButton.moc:			YQPushButton.h
YQRadioButton.moc:			YQRadioButton.h
YQRadioButtonGroup.moc:      		YQRadioButtonGroup.h
YQReplacePoint.moc:			YQReplacePoint.h
YQRichText.moc:				YQRichText.h
YQSelectionBox.moc:			YQSelectionBox.h
YQSlider.moc:				YQSlider.h
YQSpacing.moc:				YQSpacing.h
YQSplit.moc:				YQSplit.h
YQSquash.moc:				YQSquash.h
YQTable.moc:				YQTable.h
YQTextEntry.moc:			YQTextEntry.h
YQTree.moc:				YQTree.h
YQWizard.moc:				YQWizard.h
YQWizardButton.moc:			YQWizardButton.h

QY2BarGraph.moc:			QY2BarGraph.h
QY2ComboTabWidget.moc:			QY2ComboTabWidget.h
QY2DiskUsageList.moc:			QY2DiskUsageList.h
QY2FloppyMountDialog.moc:		QY2FloppyMountDialog.h
QY2CharValidator.moc:			QY2CharValidator.h
QY2ListView.moc:			QY2ListView.h

qxembed.moc:				qxembed.h


%.moc: %.h
	$(MOC) $< -o $@

CLEANFILES = *.moc

INCLUDES = -I$(srcdir)/pkg -I$(QTDIR)/include -I$(includedir) -I$(includedir)/yui ${ZYPP_CFLAGS}
