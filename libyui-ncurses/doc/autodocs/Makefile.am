
htmldir = $(docdir)/autodocs

html_DATA =				\
	NCAlignment.html		\
	NCCheckBox.html			\
	NCComboBox.html			\
	NCDialog.html			\
	NCEmpty.html			\
	NCFrame.html			\
	NCImage.html			\
	NCIntField.html			\
	NCLabel.html			\
	NCLogView.html			\
	NCMenuButton.html		\
	NCMultiLineEdit.html		\
	NCMultiSelectionBox.html	\
	NCPad.html			\
	NCPadWidget.html		\
	NCPopup.html			\
	NCPopupList.html		\
	NCPopupMenu.html		\
	NCPopupTable.html		\
	NCPopupTextEntry.html		\
	NCProgressBar.html		\
	NCPushButton.html		\
	NCRadioButton.html		\
	NCRadioButtonGroup.html		\
	NCReplacePoint.html		\
	NCRichText.html			\
	NCSchrollCB.html		\
	NCScrollHint.html		\
	NCSelectionBox.html		\
	NCSpacing.html			\
	NCSplit.html			\
	NCSquash.html			\
	NCTable.html			\
	NCTableCol.html			\
	NCTableHead.html		\
	NCTableLine.html		\
	NCTablePad.html			\
	NCTableStyle.html		\
	NCTextEntry.html		\
	NCTextPad.html			\
	NCTree.html			\
	NCTreePad.html			\
	NCWeight.html			\
	NCWidget.html			\
	NCattrcolor.html		\
	NCattribute.html		\
	NClabel.html			\
	NCstring.html			\
	NCtext.html			\
	NCurses.html			\
	NCursesColorWindow.html		\
	NCursesError.html		\
	NCursesEvent.html		\
	NCursesPad.html			\
	NCursesPanel.html		\
	NCursesUserPanel.html		\
	NCursesWindow.html		\
	Y2CCNCursesUI.html		\
	Y2NCursesUI.html		\
	Y2NCursesUIComponent.html	\
	heir.html			\
	index.html			\
	tnode.html			\
	wpair.html			\
	wpos.html			\
	wrect.html			\
	wsze.html

EXTRA_DIST = $(html_DATA)

CLEANFILES = $(html_DATA)

$(filter-out index.html, $(html_DATA)): index.html
index.html: $(srcdir)/../../src/*.h
	cd $(srcdir)/../../src && @ybindir@/ydoc @RPMNAME@ *.h
	mv -f $(srcdir)/../../src/*.html .
