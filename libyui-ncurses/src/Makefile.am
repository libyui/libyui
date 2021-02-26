##
# Makefile.am for ncurses/src
#

AM_CXXFLAGS = -DY2LOG=\"ui-ncurses\" -DLOCALEDIR=\"${localedir}\" -DNCURSES_INTERNALS=1

# plugin, libtool forces 'lib' prefix
plugin_LTLIBRARIES = libpy2ncurses.la
#noinst_LTLIBRARIES = liby2ncurses.la

libpy2ncurses_la_LDFLAGS = -version-info 2:0

libpy2ncurses_la_LIBADD =		\
	$(LIBS) -lpanelw -lyui


#libpy2ncurses_la_LIBADD =		\
#	-L$(libdir)			\
#	-Lpkg -lpy2ncurses_pkg		\
#	-lpy2UI				\
#	-lncursesw -lpanelw
#liby2ncurses_la_LIBADD = $(libpy2ncurses_la_LIBADD)

pkgincludedir = $(includedir)/ncurses

pkginclude_HEADERS = 		\
	stdutil.h           	\
	NCi18n.h		\
	tnode.h			\
	position.h		\
	YNCursesUI.h		\
	NCtoY2Event.h		\
	NCApplication.h		\
	NCWidgetFactory.h	\
	NCOptionalWidgetFactory.h	\
				\
	NCPackageSelectorPluginStub.h	\
	NCPackageSelectorPluginIf.h	\
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
	NCButtonBox.h		\
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
	NCDumbTab.h		\
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
	NCstyle.rxvt.h NCstyle.highcontrast.h	\
	NCstyle.inverted.h

install-pkgincludeHEADERS: $(pkgi/nclude_HEADERS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(pkgincludedir)
	@list='$(pkginclude_HEADERS)';                                                  \
	for p in $$list; do                                                             \
	        if test -f "$$p"; then d= ; else d="$(srcdir)/"; fi;                    \
	        if ! cmp -s $$d$$p $(DESTDIR)$(pkgincludedir)/$$p; then                 \
	                echo " $(INSTALL_DATA) $$d$$p $(DESTDIR)$(pkgincludedir)/$$p";  \
	                $(INSTALL_DATA) $$d$$p $(DESTDIR)$(pkgincludedir)/$$p;          \
	        fi                                                                      \
	done


libpy2ncurses_la_SOURCES =		\
	position.cc			\
	YNCursesUI.cc			\
	NCtoY2Event.cc			\
	NCApplication.cc		\
	NCWidgetFactory.cc		\
	NCOptionalWidgetFactory.cc	\
					\
	NCPackageSelectorPluginStub.cc	\
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
	NCButtonBox.cc			\
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
	NCDumbTab.cc			\
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
	NCAskForFile.cc


INCLUDES = -I$(includedir) -I$(includedir)/yui -I$(includedir)/ycp-ui

lang2encoding.cc:	lang2encoding.tab
	./lang2encoding.tab > lang2encoding.cc

