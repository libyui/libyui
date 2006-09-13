##
# Makefile.am for ncurses/src
#

ydata_DATA = pkg_layout.ycp	\
	you_layout.ycp

# plugin, libtool forces 'lib' prefix
AM_CXXFLAGS = -DY2LOG=\"ui-ncurses\" -DLOCALEDIR=\"${localedir}\"
AM_LDFLAGS = -L$(libdir)

plugin_LTLIBRARIES = libpy2ncurses.la
noinst_LTLIBRARIES = liby2ncurses.la

libpy2ncurses_la_LDFLAGS = -version-info 2:0
liby2ncurses_la_LDFLAGS = -version-info 2:0

libpy2ncurses_la_LIBADD =		\
	-lyui				\
	-lncursesw -lpanelw
liby2ncurses_la_LIBADD = $(libpy2ncurses_la_LIBADD)

liby2ncurses_la_SOURCES =				\
	stdutil.h					\
	NCi18n.h					\
	Y2Log.h Y2Log.cc				\
	tnode.h	position.h position.cc			\
	YNCursesUI.cc YNCursesUI.h			\
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
	NCFileSelection.h NCFileSelection.cc		\
	NCAskForDirectory.h NCAskForDirectory.cc	\
	NCAskForFile.h NCAskForFile.cc			\
							\
	NCPopup.h NCPopup.cc				\
	NCPopupTable.h NCPopupTable.cc			\
	NCPopupList.h NCPopupList.cc			\
	NCPopupMenu.h NCPopupMenu.cc			\
	NCPopupTextEntry.h NCPopupTextEntry.cc		\
							\
	YQZypp.h					\
	NCPkgSelMapper.cc NCPkgSelMapper.h		\
	NCPackageSelector.cc NCPackageSelector.h        \
	NCPkgTable.cc NCPkgTable.h                      \
	NCPopupSearch.cc NCPopupSearch.h                \
	NCPopupInfo.cc NCPopupInfo.h                    \
	PkgNames.h PkgNames.cc				\
	ObjectStatStrategy.cc ObjectStatStrategy.h      \
	NCPopupDeps.cc NCPopupDeps.h                    \
	NCPopupTree.cc NCPopupTree.h                    \
	NCPopupSelection.cc NCPopupSelection.h          \
	NCPopupDiskspace.cc NCPopupDiskspace.h          \
	NCPopupFile.cc NCPopupFile.h                    \
	NCPopupPkgTable.cc NCPopupPkgTable.h            \
	NCPopupPkgDescr.cc NCPopupPkgDescr.h            \
	NCstyle.braille.h NCstyle.mono.h		\
	NCstyle.linux.h NCstyle.xterm.h


libpy2ncurses_la_SOURCES = $(liby2ncurses_la_SOURCES)	\
	Y2CCNCursesUI.cc Y2CCNCursesUI.h	\
	YNCursesComponent.h

INCLUDES = -I$(includedir) -I$(includedir)/yui -I$(includedir)/y2util/qt ${ZYPP_CFLAGS}

lang2encoding.cc:	lang2encoding.tab
	./lang2encoding.tab > lang2encoding.cc

EXTRA_DIST	= $(ydata_DATA)
