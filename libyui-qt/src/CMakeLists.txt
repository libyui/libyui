# CMakeLists.txt for libyui-qt/src

include( ../VERSION.cmake )
include( GNUInstallDirs )       # set CMAKE_INSTALL_INCLUDEDIR, ..._LIBDIR

#
# Qt-specific
#

find_package( Qt5 COMPONENTS Widgets Svg X11Extras REQUIRED )

set( CMAKE_AUTOMOC on ) # Automatically handle "moc" preprocessor (Q_OBJECTs)
set( CMAKE_AUTORCC on ) # Automatically handle Qt resource (.rcc) files
set( CMAKE_AUTOUIC on ) # Automatically handle Qt Designer (.uic) files


#
# libyui plugin specific
#

set( TARGETLIB          libyui-qt )
set( TARGETLIB_BASE     yui-qt    )

set( HEADERS_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR}/yui/qt )
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
  YQUI.cc
  YQWidgetFactory.cc
  YQUI_builtins.cc
  
  YQAlignment.cc
  YQApplication.cc
  YQBarGraph.cc
  YQBusyIndicator.cc
  YQButtonBox.cc
  YQCheckBox.cc
  YQCheckBoxFrame.cc
  YQComboBox.cc
  YQContextMenu.cc
  YQCustomStatusItemSelector.cc
  YQDateField.cc
  YQDialog.cc
  YQDownloadProgress.cc
  YQDumbTab.cc
  YQEmpty.cc
  YQFrame.cc
  YQGenericButton.cc
  YQGraphPluginStub.cc
  YQImage.cc
  YQInputField.cc
  YQIntField.cc
  YQItemSelector.cc
  YQLabel.cc
  YQLayoutBox.cc
  YQLogView.cc
  YQMainWinDock.cc
  YQMenuBar.cc
  YQMenuButton.cc
  YQMultiLineEdit.cc
  YQMultiProgressMeter.cc
  YQMultiSelectionBox.cc
  YQOptionalWidgetFactory.cc
  YQPackageSelectorPluginStub.cc
  YQPartitionSplitter.cc
  YQProgressBar.cc
  YQPushButton.cc
  YQRadioButton.cc
  YQRadioButtonGroup.cc
  YQReplacePoint.cc
  YQRichText.cc
  YQSelectionBox.cc
  YQSignalBlocker.cc
  YQSlider.cc
  YQSpacing.cc
  YQSquash.cc
  YQTable.cc
  YQTimeField.cc
  YQTimezoneSelector.cc
  YQTree.cc
  YQWidgetCaption.cc
  YQWizard.cc
  YQWizardButton.cc
  
  QY2CharValidator.cc
  QY2ComboTabWidget.cc
  QY2DiskUsageList.cc
  QY2HelpDialog.cc
  QY2ListView.cc
  QY2RelNotesDialog.cc
  QY2StyleEditor.cc
  QY2Styler.cc
  )


set( HEADERS
  YQUI.h
  YQWidgetFactory.h
  
  YQAlignment.h
  YQApplication.h
  YQBarGraph.h
  YQBusyIndicator.h
  YQButtonBox.h
  YQCheckBox.h
  YQCheckBoxFrame.h
  YQComboBox.h
  YQContextMenu.h
  YQCustomStatusItemSelector.h
  YQDateField.h
  YQDialog.h
  YQDownloadProgress.h
  YQDumbTab.h
  YQEmpty.h
  YQFrame.h
  YQGenericButton.h
  YQGraphPluginIf.h
  YQGraphPluginStub.h
  YQImage.h
  YQInputField.h
  YQIntField.h
  YQItemSelector.h
  YQLabel.h
  YQLayoutBox.h
  YQLogView.h
  YQMainWinDock.h
  YQMenuBar.h
  YQMenuButton.h
  YQMultiLineEdit.h
  YQMultiProgressMeter.h
  YQMultiSelectionBox.h
  YQOptionalWidgetFactory.h
  YQPackageSelectorPluginIf.h
  YQPackageSelectorPluginStub.h
  YQPartitionSplitter.h
  YQProgressBar.h
  YQPushButton.h
  YQRadioButton.h
  YQRadioButtonGroup.h
  YQReplacePoint.h
  YQRichText.h
  YQSelectionBox.h
  YQSignalBlocker.h
  YQSlider.h
  YQSpacing.h
  YQSquash.h
  YQTable.h
  YQTimeField.h
  YQTimezoneSelector.h
  YQTree.h
  YQWidgetCaption.h
  YQWizard.h
  YQWizardButton.h
  YQi18n.h
  
  QY2CharValidator.h
  QY2ComboTabWidget.h
  QY2DiskUsageList.h
  QY2HelpDialog.h
  QY2ListView.h
  QY2RelNotesDialog.h
  QY2StyleEditor.h
  QY2Styler.h
  utf8.h
  )


set( UI_FILES           # Qt Designer -generated dialog descriptions
  QHelpDialog.ui
  QStyleEditor.ui
  )

set( QRC_FILES qt_icons.qrc ) # Compiled-in resources: icons


# Add shared lib to be built
add_library( ${TARGETLIB} SHARED
  ${SOURCES}
  ${HEADERS}
  ${UI_FILES}
  ${QRC_FILES}
  )

# Include headers from the sibling sub-project ../libyui:
# We include headers as public, i.e. include <yui/YFoo.h> .
# ../libyui/include has a symlink yui -> ../src which makes
# the libyui headers accessible as yui/YFoo.h
target_include_directories( ${TARGETLIB} BEFORE PUBLIC ../../libyui/include )

# Make the version from ../VERSION.cmake available as a #define
target_compile_definitions( ${TARGETLIB} PUBLIC VERSION="${VERSION}" )

# Libraries that are needed to build this shared lib
target_link_libraries( ${TARGETLIB}
  yui
  Qt5::Core
  Qt5::Gui
  Qt5::Widgets
  Qt5::Svg
  Qt5::X11Extras
  )


# Add more compile options to this target in addition to those
# added in the toplevel CMakeLists.txt.
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
