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
	YQPackageSelector.h			\
	YQPartitionSplitter.h			\
	YQPkgChangesDialog.h			\
	YQPkgConflictDialog.h			\
	YQPkgConflictList.h			\
	YQPkgDependenciesView.h			\
	YQPkgDiskUsageList.h			\
	YQPkgDiskUsageWarningDialog.h		\
	YQPkgDescriptionView.h			\
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
						\
	QY2BarGraph.h				\
	QY2ComboTabWidget.h			\
	QY2DiskUsageList.h			\
	QY2CharValidator.h			\
	QY2ListView.h				\
	QXEmbed.h				\
						\
	autodel.xpm				\
	autodel_disabled.xpm			\
	autoinstall.xpm				\
	autoinstall_disabled.xpm		\
	autoupdate.xpm				\
	autoupdate_disabled.xpm			\
	checklist_off.xpm			\
	checklist_on.xpm			\
	del.xpm					\
	del_disabled.xpm			\
	delete_conflict.xpm			\
	install.xpm				\
	install_disabled.xpm			\
	keepinstalled.xpm			\
	keepinstalled_disabled.xpm		\
	noinst.xpm				\
	noinst_disabled.xpm			\
	normal_conflict.xpm			\
	select_conflict.xpm			\
	sel_autodel.xpm				\
	sel_autoinstall.xpm			\
	sel_autoupdate.xpm			\
	stop.xpm				\
	taboo.xpm				\
	taboo_disabled.xpm			\
	unresolvable_conflict.xpm		\
	update.xpm				\
	update_disabled.xpm



# Those icons are used at runtime by the online help system.
# For everything else, they are included in the source code -
# no need to install all the other icons as well.
#
# -sh 2003-01-14

helpimagesdir = $(imagedir)

helpimages_DATA =				\
	autodel.xpm				\
	autoinstall.xpm				\
	autoupdate.xpm				\
	del.xpm					\
	install.xpm				\
	keepinstalled.xpm			\
	noinst.xpm				\
	taboo.xpm				\
	update.xpm

EXTRA_DIST = $(helpimages_DATA)

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
