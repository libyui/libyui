#
# Makefile.am for qt/src/include
#

pkgincludedir = $(includedir)/y2qt

pkginclude_HEADERS =				\
	layoututils.h				\
	utf8.h					\
	YQi18n.h				\
	YUIQt.h					\
	Y2CCQt.h				\
	Y2QtComponent.h				\
	YQWidget.h				\
	YQIconPool.h				\
						\
	YQAlignment.h				\
	YQBarGraph.h				\
	YQCheckBox.h				\
	YQColoredLabel.h			\
	YQComboBox.h				\
	YQDialog.h				\
	YQDownloadProgress.h			\
	YQEmpty.h				\
	YQFrame.h				\
	YQImage.h				\
	YQIntField.h				\
	YQLabel.h				\
	YQLogView.h				\
	YQMenuButton.h				\
	YQMultiLineEdit.h			\
	YQMultiSelectionBox.h			\
	YQPartitionSplitter.h			\
	YQPackageSelector.h			\
	YQPkgConflictDialog.h			\
	YQPkgConflictList.h			\
	YQPkgGenericDetailsView.h		\
	YQPkgDescriptionView.h			\
	YQPkgTechnicalDetailsView.h		\
	YQPkgObjList.h				\
	YQPkgList.h				\
	YQPkgRpmGroupTagsFilterView.h		\
	YQPkgSelectionsFilterView.h		\
	YQPkgSelList.h				\
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
						\
	QY2BarGraph.h				\
	QY2ComboTabWidget.h			\
	QY2CharValidator.h			\
	QY2ListView.h				\
	QXEmbed.h				\
						\
	checklist_off.xpm			\
	checklist_on.xpm			\
	recycle.xpm				\
	taboo.xpm				\
	autoinst.xpm				\
	trashcan.xpm				\
	dep.xpm


CLEANFILES = y2qt.kdoc

#<INSTALL-HEADER-TARGET>

# Automatically inserted by insert_install_target
# DO NOT REMOVE THE TAGS!

# Install header files only if different to version already installed -
# prevent unnecessary installations to keep make dependency changes
# and thus rebuilds for dependent packages as low as possible.

install-pkgincludeHEADERS: $(pkginclude_HEADERS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(pkgincludedir)
	@list='$(pkginclude_HEADERS)';							\
	for p in $$list; do								\
		if test -f "$$p"; then d= ; else d="$(srcdir)/"; fi; 			\
		if ! cmp -s $$d$$p $(DESTDIR)$(pkgincludedir)/$$p; then			\
			echo " $(INSTALL_DATA) $$d$$p $(DESTDIR)$(pkgincludedir)/$$p";	\
			$(INSTALL_DATA) $$d$$p $(DESTDIR)$(pkgincludedir)/$$p;		\
	  	fi									\
	done

#</INSTALL-HEADER-TARGET>
