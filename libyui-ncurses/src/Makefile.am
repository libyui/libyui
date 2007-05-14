##
# Makefile.am for ncurses/src
#

SUBDIRS = pkg

AM_CXXFLAGS = -DY2LOG=\"ui-ncurses\" -DLOCALEDIR=\"${localedir}\"
AM_LDFLAGS = -L$(libdir)

# plugin, libtool forces 'lib' prefix
plugin_LTLIBRARIES = libpy2ncurses.la
#noinst_LTLIBRARIES = liby2ncurses.la

libpy2ncurses_la_LDFLAGS = -version-info 2:0
#liby2ncurses_la_LDFLAGS = -version-info 2:0

libpy2ncurses_la_LIBADD =		\
	-L$(libdir)			\
	-Lpkg -lpy2ncurses_pkg		\
	-lyui				\
	-lncursesw -lpanelw
#liby2ncurses_la_LIBADD = $(libpy2ncurses_la_LIBADD)

libpy2ncurses_la_SOURCES =				\
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
	NCCheckBoxFrame.h NCCheckBoxFrame.cc		\
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
	NCPopupInfo.h NCPopupInfo.cc			\
							\
	NCstyle.braille.h NCstyle.mono.h		\
	NCstyle.linux.h NCstyle.xterm.h			\
							\
	Y2CCNCursesUI.cc Y2CCNCursesUI.h		\
	YNCursesComponent.h

#libpy2ncurses_la_SOURCES = $(liby2ncurses_la_SOURCES)	\
#	Y2CCNCursesUI.cc Y2CCNCursesUI.h	\
#	YNCursesComponent.h

INCLUDES = -I$(srcdir)/pkg -I$(includedir) -I$(includedir)/yui -I$(includedir)/y2util/qt ${ZYPP_CFLAGS}

lang2encoding.cc:	lang2encoding.tab
	./lang2encoding.tab > lang2encoding.cc

