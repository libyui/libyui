
htmldir = $(docdir)/autodocs

html_DATA =					\
	Y2CCQt.html				\
	YUIQt.html				\
	heir.html				\
	index.html				\
						\
	QY2BarGraph.html			\
	Y2CCQt.html				\
	Y2QtComponent.html			\
	YQAlignment.html			\
	YQBarGraph.html				\
	YQCheckBox.html				\
	YQComboBox.html				\
	YQDialog.html				\
	YQEmpty.html				\
	YQFrame.html				\
	YQImage.html				\
	YQIntField.html				\
	YQLabel.html				\
	YQLogView.html				\
	YQMultiLineEdit.html			\
	YQPartitionSplitter.html		\
	YQProgressBar.html			\
	YQPushButton.html			\
	YQRadioButton.html			\
	YQRadioButtonGroup.html			\
	YQReplacePoint.html			\
	YQRichText.html				\
	YQSelectionBox.html			\
	YQSlider.html				\
	YQSpacing.html				\
	YQSplit.html				\
	YQSquash.html				\
	YQTable.html				\
	YQTextEntry.html			\
	YQTree.html				\
	YUIQt.html

EXTRA_DIST = $(html_DATA)

CLEANFILES = $(html_DATA)

$(filter-out index.html, $(html_DATA)): index.html
index.html: $(srcdir)/../../src/include/*.h
	cd $(srcdir)/../../src/include && @ybindir@/ydoc @RPMNAME@ *.h
	mv -f $(srcdir)/../../src/include/*.html .
