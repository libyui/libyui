# CMakeLists.txt for libyui-qt-pkg/src

include( ../VERSION.cmake )
include( GNUInstallDirs )       # set CMAKE_INSTALL_INCLUDEDIR, ..._LIBDIR

#
# Qt-specific
#

find_package( Qt5 COMPONENTS Widgets Svg X11Extras REQUIRED )

set( CMAKE_AUTOMOC on ) # Automatically handle "moc" preprocessor (Q_OBJECTs)
set( CMAKE_AUTORCC on ) # Automatically handle Qt resource (.rcc) files
set( CMAKE_AUTOUIC on ) # Automatically handle Qt Designer (.uic) files

# find_library( zypp ) is pointless because there is a libzypp on every SUSE


#
# libyui plugin specific
#

set( TARGETLIB          libyui-qt-pkg )
set( TARGETLIB_BASE     yui-qt-pkg    )

set( HEADERS_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR}/yui/qt-pkg )
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
  YQPackageSelector.cc
  YQPackageSelectorBase.cc
  YQPackageSelectorHelp.cc
  YQPatternSelector.cc
  YQSimplePatchSelector.cc
  YQPackageSelectorPluginImpl.cc
  
  YQPkgChangeLogView.cc
  YQPkgChangesDialog.cc
  YQPkgClassFilterView.cc
  YQPkgConflictDialog.cc
  YQPkgConflictList.cc
  YQPkgDependenciesView.cc
  YQPkgDescriptionDialog.cc
  YQPkgDescriptionView.cc
  YQPkgDiskUsageList.cc
  YQPkgDiskUsageWarningDialog.cc
  YQPkgFileListView.cc
  YQPkgFilterTab.cc
  YQPkgFilters.cc
  YQPkgGenericDetailsView.cc
  YQPkgHistoryDialog.cc
  YQPkgLangList.cc
  YQPkgList.cc
  YQPkgObjList.cc
  YQPkgPatchFilterView.cc
  YQPkgPatchList.cc
  YQPkgPatternList.cc
  YQPkgProductDialog.cc
  YQPkgProductList.cc
  YQPkgRepoFilterView.cc
  YQPkgRepoList.cc
  YQPkgSearchFilterView.cc
  YQPkgSecondaryFilterView.cc
  YQPkgSelDescriptionView.cc
  YQPkgSelMapper.cc
  YQPkgServiceFilterView.cc
  YQPkgServiceList.cc
  YQPkgStatusFilterView.cc
  YQPkgTechnicalDetailsView.cc
  YQPkgTextDialog.cc
  YQPkgUpdateProblemFilterView.cc
  YQPkgVersionsView.cc
  
  YQIconPool.cc
  QY2LayoutUtils.cc
  )


set( HEADERS
  YQPackageSelector.h
  YQPackageSelectorBase.h
  YQPackageSelectorPluginImpl.h
  YQPatternSelector.h
  YQSimplePatchSelector.h
  YQPackageSelectorPlugin.h
  
  YQPkgChangeLogView.h
  YQPkgChangesDialog.h
  YQPkgClassFilterView.h
  YQPkgConflictDialog.h
  YQPkgConflictList.h
  YQPkgDependenciesView.h
  YQPkgDescriptionDialog.h
  YQPkgDescriptionView.h
  YQPkgDiskUsageList.h
  YQPkgDiskUsageWarningDialog.h
  YQPkgFileListView.h
  YQPkgFilterTab.h
  YQPkgFilters.h
  YQPkgGenericDetailsView.h
  YQPkgHistoryDialog.h
  YQPkgLangList.h
  YQPkgList.h
  YQPkgObjList.h
  YQPkgPatchFilterView.h
  YQPkgPatchList.h
  YQPkgPatternList.h
  YQPkgProductDialog.h
  YQPkgProductList.h
  YQPkgRepoFilterView.h
  YQPkgRepoList.h
  YQPkgSearchFilterView.h
  YQPkgSecondaryFilterView.h
  YQPkgSelDescriptionView.h
  YQPkgSelList.h
  YQPkgSelMapper.h
  YQPkgServiceFilterView.h
  YQPkgServiceList.h
  YQPkgStatusFilterView.h
  YQPkgTechnicalDetailsView.h
  YQPkgTextDialog.h
  YQPkgUpdateProblemFilterView.h
  YQPkgVersionsView.h
  
  QY2LayoutUtils.h
  YQIconPool.h
  YQZypp.h
  )


set( QRC_FILES qt_pkg_icons.qrc ) # Compiled-in resources: icons


# Add shared lib to be built
add_library( ${TARGETLIB} SHARED
  ${SOURCES}
  ${HEADERS}
  ${QRC_FILES}
  )

# Include headers from the sibling sub-project ../libyui:
# We include headers as public, i.e. include <yui/YFoo.h> .
# ../libyui/include has a symlink yui -> ../src which makes
# the libyui headers accessible as yui/YFoo.h
target_include_directories( ${TARGETLIB} BEFORE PUBLIC ../../libyui/include )
target_include_directories( ${TARGETLIB} BEFORE PUBLIC ../../libyui-qt/include )

# Libraries that are needed to build this shared lib
target_link_libraries( ${TARGETLIB}
  yui
  zypp
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
