##
# Makefile.am for ncurses/src
#

SUBDIRS = pkg

AM_CXXFLAGS = -DY2LOG=\"ui-ncurses\" -DLOCALEDIR=\"${localedir}\"

# plugin, libtool forces 'lib' prefix
plugin_LTLIBRARIES = libpy2ncurses.la
#noinst_LTLIBRARIES = liby2ncurses.la

libpy2ncurses_la_LDFLAGS = -version-info 2:0

libpy2ncurses_la_LIBADD =		\
	-L$(plugindir)			\
	-lpy2UI				\
	-lncursesw -lpanelw

#libpy2ncurses_la_LIBADD =		\
#	-L$(libdir)			\
#	-Lpkg -lpy2ncurses_pkg		\
#	-lpy2UI				\
#	-lncursesw -lpanelw
#liby2ncurses_la_LIBADD = $(libpy2ncurses_la_LIBADD)

noinst_HEADERS = 		\
	stdutil.h           	\
	NCi18n.h		\
	Y2Log.h			\
	tnode.h			\
	position.h		\
	YNCursesUI.h		\
	NCApplication.h		\
	NCWidgetFactory.h	\
	NCOptionalWidgetFactory.h	\
				\
	NCPackageSelectorPlugin.h	\
	NCPackageSelectorStart.h	\
	NCPkgTable.h		\
	NCtext.h		\
	NCstring.h		\
	stringutil.h		\
	ncursesw.h		\
	ncursesp.h		\
	NCtypes.h		\
	NCurses.h		\
	NCstyle.h		\
	NCStyleDef.h		\
	NCTableItem.h       	\
	NCPad.h     		\
	NCTreePad.h     	\
	NCTablePad.h        	\
	NCTextPad.h     	\
	NCWidget.h      	\
	NCDialog.h      	\
	NCEmpty.h       	\
	NCSpacing.h     	\
	NCSquash.h      	\
	NCAlignment.h       	\
	NCFrame.h       	\
	NCCheckBoxFrame.h       \
	NCLayoutBox.h		\
	NCReplacePoint.h        \
	NCRadioButtonGroup.h	\
	NCImage.h       	\
	NCCheckBox.h        	\
	NCLabel.h       	\
	NCProgressBar.h     	\
	NCBusyIndicator.h     	\
	NCMenuButton.h      	\
	NCPushButton.h      	\
	NCRadioButton.h     	\
	NCInputField.h       	\
	NCIntField.h        	\
	NCComboBox.h        	\
	NCPadWidget.h       	\
	NCRichText.h        	\
	NCSelectionBox.h        \
	NCMultiSelectionBox.h	\
	NCTable.h       	\
	NCTree.h        	\
	NCLogView.h     	\
	NCMultiLineEdit.h       \
	NCFileSelection.h       \
	NCPopup.h       	\
	NCPopupTable.h      	\
	NCPopupList.h       	\
	NCPopupMenu.h       	\
	NCPopupTextEntry.h      \
	NCPopupInfo.h       	\
	NCAskForDirectory.h     \
	NCAskForFile.h		\
	NCstyle.braille.h NCstyle.mono.h	\
	NCstyle.linux.h NCstyle.xterm.h		\
	NCstyle.rxvt.h				\
	Y2CCNCursesUI.h				\
	YNCursesComponent.h

libpy2ncurses_la_SOURCES =		\
	Y2Log.cc			\
	position.cc			\
	YNCursesUI.cc			\
	NCApplication.cc		\
	NCWidgetFactory.cc		\
	NCOptionalWidgetFactory.cc	\
					\
	NCPackageSelectorPlugin.cc	\
	NCPackageSelectorStart.cc 	\
	NCPkgTable.cc			\
					\
	NCtext.cc			\
	NCstring.cc			\
	stringutil.cc			\
	lang2encoding.tab lang2encoding.cc	\
	ncursesw.cc			\
	ncursesp.cc			\
	NCtypes.cc			\
	NCurses.cc			\
	NCstyle.cc			\
	NCStyleDef.cc			\
	NCTableItem.cc			\
	NCPad.cc			\
	NCTreePad.cc			\
	NCTablePad.cc			\
	NCTextPad.cc			\
	NCWidget.cc			\
	NCDialog.cc			\
	NCEmpty.cc			\
	NCSpacing.cc			\
	NCSquash.cc			\
	NCAlignment.cc			\
	NCFrame.cc			\
	NCCheckBoxFrame.cc		\
	NCLayoutBox.cc			\
	NCReplacePoint.cc		\
	NCRadioButtonGroup.cc		\
	NCImage.cc			\
	NCCheckBox.cc			\
	NCLabel.cc			\
	NCProgressBar.cc		\
	NCBusyIndicator.cc		\
	NCMenuButton.cc			\
	NCPushButton.cc			\
	NCRadioButton.cc		\
	NCInputField.cc			\
	NCIntField.cc			\
	NCComboBox.cc			\
	NCPadWidget.cc			\
	NCRichText.cc			\
	NCSelectionBox.cc		\
	NCMultiSelectionBox.cc		\
	NCTable.cc			\
	NCTree.cc			\
	NCLogView.cc			\
	NCMultiLineEdit.cc		\
	NCFileSelection.cc		\
					\
	NCPopup.cc			\
	NCPopupTable.cc			\
	NCPopupList.cc			\
	NCPopupMenu.cc			\
	NCPopupTextEntry.cc		\
	NCPopupInfo.cc			\
	NCAskForDirectory.cc		\
	NCAskForFile.cc			\
					\
	Y2CCNCursesUI.cc


#libpy2ncurses_la_SOURCES = $(liby2ncurses_la_SOURCES)	\
#	Y2CCNCursesUI.cc Y2CCNCursesUI.h	\
#	YNCursesComponent.h

INCLUDES = -I$(srcdir)/pkg -I$(includedir) -I$(includedir)/yui -I$(includedir)/y2util/qt ${ZYPP_CFLAGS}

lang2encoding.cc:	lang2encoding.tab
	./lang2encoding.tab > lang2encoding.cc

