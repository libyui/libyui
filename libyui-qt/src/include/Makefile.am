#
# Makefile.am for qt/src/include
#

SUBDIRS = icons

pkgincludedir = $(includedir)/y2qt

pkginclude_HEADERS =				\
	YQUI.h

noinst_HEADERS =				\
	utf8.h					\
	YQi18n.h				\
	Y2CCQt.h				\
	YQUIComponent.h				\
	YQIconPool.h				\
						\
	YQAlignment.h				\
	YQBarGraph.h				\
	YQCheckBox.h				\
	YQColoredLabel.h			\
	YQComboBox.h				\
	YQDialog.h				\
	YQDownloadProgress.h			\
	YQDumbTab.h				\
	YQEBunny.h				\
	YQEmpty.h				\
	YQFrame.h				\
	YQGenericButton.h			\
	YQImage.h				\
	YQIntField.h				\
	YQLabel.h				\
	YQLogView.h				\
	YQMenuButton.h				\
	YQMultiLineEdit.h			\
	YQMultiSelectionBox.h			\
	YQPackageSelector.h			\
	YQPartitionSplitter.h			\
	YQPkgChangesDialog.h			\
	YQPkgConflictDialog.h			\
	YQPkgConflictList.h			\
	YQPkgDependenciesView.h			\
	YQPkgDescriptionDialog.h		\
	YQPkgDescriptionView.h			\
	YQPkgDiskUsageList.h			\
	YQPkgDiskUsageWarningDialog.h		\
	YQPkgGenericDetailsView.h		\
	YQPkgList.h				\
	YQPkgObjList.h				\
	YQPkgRpmGroupTagsFilterView.h		\
	YQPkgSearchFilterView.h			\
	YQPkgSelList.h				\
	YQPkgSelectionsFilterView.h		\
	YQPkgStatusFilterView.h			\
	YQPkgTechnicalDetailsView.h		\
	YQPkgTextDialog.h			\
	YQPkgUpdateProblemFilterView.h		\
	YQPkgVersionsView.h			\
	YQPkgYouPatchFilterView.h		\
	YQPkgYouPatchList.h			\
	YQProgressBar.h				\
	YQPushButton.h				\
	YQRadioButton.h				\
	YQRadioButtonGroup.h			\
	YQReplacePoint.h			\
	YQRichText.h				\
	YQSelectionBox.h			\
	YQSlider.h				\
	YQSpacing.h				\
	YQSplit.h				\
	YQSquash.h				\
	YQTable.h				\
	YQTextEntry.h				\
	YQTree.h				\
	YQWizard.h				\
	YQWizardButton.h			\
						\
	YFrameBuffer.h				\
	QY2BarGraph.h				\
	QY2CharValidator.h			\
	QY2ComboTabWidget.h			\
	QY2DiskUsageList.h			\
	QY2FloppyMountDialog.h			\
	QY2ListView.h				\
	QY2LayoutUtils.h			\
						\
	qxembed.h


CLEANFILES = y2qt.kdoc

#<INSTALL-HEADER-TARGET>

# Automatically inserted by insert_install_target
# DO NOT REMOVE THE TAGS!

# Install header files only if different to version already installed -
# prevent unnecessary installations to keep make dependency changes
# and thus rebuilds for dependent packages as low as possible.

###	install-pkgincludeHEADERS: $(pkginclude_HEADERS)
###		@$(NORMAL_INSTALL)
###		$(mkinstalldirs) $(DESTDIR)$(pkgincludedir)
###		@list='$(pkginclude_HEADERS)';							\
###		for p in $$list; do								\
###			if test -f "$$p"; then d= ; else d="$(srcdir)/"; fi; 			\
###			if ! cmp -s $$d$$p $(DESTDIR)$(pkgincludedir)/$$p; then			\
###				echo " $(INSTALL_DATA) $$d$$p $(DESTDIR)$(pkgincludedir)/$$p";	\
###				$(INSTALL_DATA) $$d$$p $(DESTDIR)$(pkgincludedir)/$$p;		\
###		  	fi									\
###		done

#</INSTALL-HEADER-TARGET>
