##
# Makefile.am for ncurses/src
#

ydata_DATA = pkg_layout.ycp	\
	you_layout.ycp

# plugin, libtool forces 'lib' prefix
AM_CXXFLAGS = -DY2LOG=\"ui-ncurses\" -DLOCALEDIR=\"${localedir}\"
plugin_LTLIBRARIES = libpy2ncurses.la
noinst_LTLIBRARIES = liby2ncurses.la

libpy2ncurses_la_LDFLAGS = -version-info 2:0
liby2ncurses_la_LDFLAGS = -version-info 2:0

libpy2ncurses_la_LIBADD =		\
	-lncursesw -lpanelw -ly2pm
liby2ncurses_la_LIBADD = $(libpy2ncurses_la_LIBADD)

liby2ncurses_la_SOURCES =				\
	stdutil.h					\
	NCi18n.h					\
	Y2NCursesUI.h Y2NCursesUI.cc			\
	Y2Log.h Y2Log.cc				\
	tnode.h	position.h position.cc			\
	NCtext.h NCtext.cc				\
	NCstring.h NCstring.cc				\
	stringutil.h stringutil.cc			\
	lang2encoding.tab lang2encoding.cc		\
	ncursesw.h ncursesw.cc				\
	ncursesp.h ncursesp.cc				\
	PackageSelector.h PackageSelector.cc		\
	NCtypes.h NCtypes.cc				\
	NCurses.h NCurses.cc				\
	NCstyle.h NCstyle.cc				\
	NCStyleDef.h NCStyleDef.cc			\
	NCTableItem.h NCTableItem.cc			\
	NCPad.h NCPad.cc				\
	NCTreePad.h NCTreePad.cc			\
	NCTablePad.h NCTablePad.cc			\
	NCTextPad.h NCTextPad.cc			\
	NCWidget.h NCWidget.cc				\
	NCDialog.h NCDialog.cc				\
	NCEmpty.h NCEmpty.cc				\
	NCSpacing.h NCSpacing.cc			\
	NCSquash.h NCSquash.cc				\
	NCAlignment.h NCAlignment.cc			\
	NCFrame.h NCFrame.cc				\
	NCSplit.h NCSplit.cc				\
	NCReplacePoint.h NCReplacePoint.cc		\
	NCRadioButtonGroup.h NCRadioButtonGroup.cc	\
							\
	NCImage.h NCImage.cc				\
	NCCheckBox.h NCCheckBox.cc			\
	NCLabel.h NCLabel.cc				\
	NCProgressBar.h NCProgressBar.cc		\
	NCMenuButton.h NCMenuButton.cc			\
	NCPushButton.h NCPushButton.cc			\
	NCRadioButton.h NCRadioButton.cc		\
	NCTextEntry.h NCTextEntry.cc			\
	NCIntField.h NCIntField.cc			\
	NCComboBox.h NCComboBox.cc			\
	NCPadWidget.h NCPadWidget.cc			\
	NCRichText.h NCRichText.cc			\
	NCSelectionBox.h NCSelectionBox.cc		\
	NCMultiSelectionBox.h NCMultiSelectionBox.cc	\
	NCTable.h NCTable.cc				\
	NCTree.h NCTree.cc				\
	NCLogView.h NCLogView.cc			\
	NCMultiLineEdit.h NCMultiLineEdit.cc		\
	NCAskForExistingDirectory.h NCAskForExistingDirectory.cc	\
									\
	NCPopup.h NCPopup.cc				\
	NCPopupTable.h NCPopupTable.cc			\
	NCPopupList.h NCPopupList.cc			\
	NCPopupMenu.h NCPopupMenu.cc			\
	NCPopupTextEntry.h NCPopupTextEntry.cc		\
	NCPackageSelector.cc NCPackageSelector.h	\
	NCPkgTable.cc NCPkgTable.h			\
	NCPopupTree.cc NCPopupTree.h			\
	NCPopupSearch.cc NCPopupSearch.h		\
	NCPopupInfo.cc NCPopupInfo.h			\
	NCPopupSelection.cc NCPopupSelection.h		\
	NCPopupDeps.cc NCPopupDeps.h			\
	NCPopupDiskspace.cc NCPopupDiskspace.h		\
	NCPopupPkgDeps.cc NCPopupPkgDeps.h		\
	NCPopupSelDeps.cc NCPopupSelDeps.h		\
	NCPopupFile.cc NCPopupFile.h			\
	NCPopupPkgTable.cc NCPopupPkgTable.h		\
	NCPopupPkgDescr.cc NCPopupPkgDescr.h		\
	PkgNames.cc PkgNames.h				\
	ObjectStatStrategy.cc ObjectStatStrategy.h	\
	NCstyle.braille.h NCstyle.mono.h 		\
	NCstyle.linux.h NCstyle.xterm.h


libpy2ncurses_la_SOURCES = $(liby2ncurses_la_SOURCES)	\
	Y2CCNCursesUI.cc Y2CCNCursesUI.h	\
	Y2NCursesUIComponent.cc Y2NCursesUIComponent.h

INCLUDES = -I$(includedir) -I$(includedir)/yui -I$(includedir)/y2util/qt

lang2encoding.cc:	lang2encoding.tab
	lang2encoding.tab > lang2encoding.cc

EXTRA_DIST	= $(ydata_DATA)
