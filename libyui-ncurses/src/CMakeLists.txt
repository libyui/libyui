# CMakeLists.txt for libyui-ncurses/src

include( ../VERSION.cmake )
include( GNUInstallDirs )       # set CMAKE_INSTALL_INCLUDEDIR, ..._LIBDIR


# Check if the libs we link against are available.
# They are all part of package ncurses-devel).

find_library( NCURSESW_LIB NAMES ncursesw REQUIRED )
find_library( PANELW_LIB   NAMES panelw   REQUIRED )
find_library( TINFO_LIB    NAMES tinfo    REQUIRED )

set( NCURSES_LIBS ${NCURSESW_LIB} ${PANELW_LIB} ${TINFO_LIB} )


#
# libyui plugin specific
#

set( TARGETLIB          libyui-ncurses )
set( TARGETLIB_BASE     yui-ncurses    )

set( HEADERS_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR}/yui/ncurses )
set( PLUGIN_DIR          ${CMAKE_INSTALL_LIBDIR}/yui ) # /usr/lib64/yui

# if DESTDIR is set, CMAKE_INSTALL_INCLUDEDIR already contains it
# during "make install" (but not for other make targets!):
#
#    sudo make install DESTDIR=/work/foo
# or
#    DESTDIR=/work/foo sudo make install
#
# -> the include files are installed to /work/foo/usr/include/...
# We need that for RPM builds to install everything to $RPM_BUILD_ROOT.


set( SOURCES
  YNCursesUI.cc
  NCApplication.cc
  NCWidgetFactory.cc
  NCOptionalWidgetFactory.cc

  NCurses.cc
  NCStyleDef.cc
  NCstring.cc
  NCstyle.cc
  NCtext.cc
  NCtoY2Event.cc
  NCtypes.cc
  lang2encoding.cc
  ncursesp.cc
  ncursesw.cc
  position.cc
  stringutil.cc

  NCAlignment.cc
  NCAskForDirectory.cc
  NCAskForFile.cc
  NCBusyIndicator.cc
  NCButtonBox.cc
  NCCheckBox.cc
  NCCheckBoxFrame.cc
  NCComboBox.cc
  NCCustomStatusItemSelector.cc
  NCDateField.cc
  NCDialog.cc
  NCDumbTab.cc
  NCEmpty.cc
  NCFileSelection.cc
  NCFrame.cc
  NCImage.cc
  NCInputField.cc
  NCInputTextBase.cc
  NCIntField.cc
  NCItemSelector.cc
  NCLabel.cc
  NCLayoutBox.cc
  NCLogView.cc
  NCMenuBar.cc
  NCMenuButton.cc
  NCMultiLineEdit.cc
  NCMultiSelectionBox.cc
  NCPackageSelectorPluginStub.cc
  NCPad.cc
  NCPadWidget.cc
  NCPopup.cc
  NCPopupInfo.cc
  NCPopupList.cc
  NCPopupMenu.cc
  NCPopupTable.cc
  NCPopupTextEntry.cc
  NCProgressBar.cc
  NCPushButton.cc
  NCRadioButton.cc
  NCRadioButtonGroup.cc
  NCReplacePoint.cc
  NCRichText.cc
  NCSelectionBox.cc
  NCSpacing.cc
  NCSquash.cc
  NCTable.cc
  NCTableItem.cc
  NCTablePad.cc
  NCTablePadBase.cc
  NCTableSort.cc
  NCTextPad.cc
  NCTimeField.cc
  NCTree.cc
  NCTreePad.cc
  NCWidget.cc
  NCWordWrapper.cc
  )


set( HEADERS
  YNCursesUI.h
  NCApplication.h
  NCWidgetFactory.h
  NCOptionalWidgetFactory.h

  NCurses.h
  NCi18n.h
  NCstring.h
  NCstyle.h
  NCtext.h
  NCtoY2Event.h
  NCtypes.h
  CyclicContainer.h
  ncursesp.h
  ncursesw.h
  position.h
  stdutil.h
  stringutil.h
  tnode.h

  NCAlignment.h
  NCAskForDirectory.h
  NCAskForFile.h
  NCBusyIndicator.h
  NCButtonBox.h
  NCCheckBox.h
  NCCheckBoxFrame.h
  NCComboBox.h
  NCCustomStatusItemSelector.h
  NCDateField.h
  NCDialog.h
  NCDumbTab.h
  NCEmpty.h
  NCFileSelection.h
  NCFrame.h
  NCImage.h
  NCInputField.h
  NCInputTextBase.h
  NCIntField.h
  NCItemSelector.h
  NCLabel.h
  NCLayoutBox.h
  NCLogView.h
  NCMenuBar.h
  NCMenuButton.h
  NCMultiLineEdit.h
  NCMultiSelectionBox.h
  NCPackageSelectorPluginIf.h
  NCPackageSelectorPluginStub.h
  NCPad.h
  NCPadWidget.h
  NCPopup.h
  NCPopupInfo.h
  NCPopupList.h
  NCPopupMenu.h
  NCPopupTable.h
  NCPopupTextEntry.h
  NCProgressBar.h
  NCPushButton.h
  NCRadioButton.h
  NCRadioButtonGroup.h
  NCReplacePoint.h
  NCRichText.h
  NCSelectionBox.h
  NCSpacing.h
  NCSquash.h
  NCStyleDef.h
  NCTable.h
  NCTableItem.h
  NCTablePad.h
  NCTablePadBase.h
  NCTableSort.h
  NCTextPad.h
  NCTimeField.h
  NCTree.h
  NCTreePad.h
  NCWidget.h
  NCWordWrapper.h

  NCstyle.braille.h
  NCstyle.highcontrast.h
  NCstyle.inverted.h
  NCstyle.linux.h
  NCstyle.mono.h
  NCstyle.rxvt.h
  NCstyle.xterm.h
  )



# Add shared lib to be built
add_library( ${TARGETLIB} SHARED ${SOURCES} ${HEADERS} )


# Include headers from the sibling sub-project ../libyui:
# We include headers as public, i.e. include <yui/YFoo.h> .
# ../libyui/include has a symlink yui -> ../src which makes
# the libyui headers accessible as yui/YFoo.h
target_include_directories( ${TARGETLIB} BEFORE PUBLIC ../../libyui/include )

# Libraries that are needed to build this shared lib
target_link_libraries( ${TARGETLIB} yui ${NCURSES_LIBS} )

# Allow operator<<() to output internal values of NCurses WINDOW etc.
target_compile_definitions( ${TARGETLIB} PUBLIC NCURSES_INTERNALS=1 )


# Add more compile options to this target in addition to those
# added in the toplevel CMakeLists.txt and target_compile_definitions().
#
# Notice that CMake will automatically add -fPIC etc. where needed,
# like for this shared lib.
### target_compile_options( ${TARGETLIB} PUBLIC "-Dfoo" )

# Show the complete compiler commands with all arguments:
#   make VERBOSE=1

# Add more compile options to an individual source file:
### set_source_files_properties( YUI.cc PROPERTIES COMPILE_OPTIONS "-Dfoo" )


# https://cmake.org/cmake/help/latest/manual/cmake-properties.7.html#target-properties
set_target_properties( ${TARGETLIB} PROPERTIES
  VERSION       ${SONAME}           # From ../VERSION.cmake
  SOVERSION     ${SONAME_MAJOR}     # From ../VERSION.cmake
  OUTPUT_NAME   ${TARGETLIB_BASE}
  )


# Install the headers first so the message about the lib does not scroll away
install( FILES   ${HEADERS}   DESTINATION ${HEADERS_INSTALL_DIR} )
install( TARGETS ${TARGETLIB} LIBRARY DESTINATION ${PLUGIN_DIR} )

# This script is collected in the .spec file as the contents of the -tools subpackage
install( PROGRAMS libyui-terminal DESTINATION bin )


### # Generate Libyui_ncurses_config.h where some CMake variables are expanded
### # for use in the C++ code. This file is NOT installed upon "make install".
###
### configure_file( Libyui_ncurses_config.h.in Libyui_ncurses_config.h )
### include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
