# CMakeLists.txt for libyui-ncurses/src

include( ../VERSION.cmake )
include( GNUInstallDirs )       # set CMAKE_INSTALL_INCLUDEDIR, ..._LIBDIR


# Check if the libs we link against are available.
# They are all part of package ncurses-devel.

find_library( NCURSESW_LIB NAMES ncursesw REQUIRED )
find_library( PANELW_LIB   NAMES panelw   REQUIRED )
find_library( TINFO_LIB    NAMES tinfo    REQUIRED )

set( NCURSES_LIBS ${NCURSESW_LIB} ${PANELW_LIB} ${TINFO_LIB} )


#
# libyui plugin specific
#

set( TARGETLIB          libyui-ncurses-pkg )
set( TARGETLIB_BASE     yui-ncurses-pkg    )

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
  NCPackageSelector.cc
  NCPackageSelectorPluginImpl.cc
  NCPackageSelectorStart.cc
  
  NCPkgFilterClassification.cc
  NCPkgFilterInstSummary.cc
  NCPkgFilterLocale.cc
  NCPkgFilterMain.cc
  NCPkgFilterPattern.cc
  NCPkgFilterRepo.cc
  NCPkgFilterSearch.cc
  NCPkgFilterService.cc
  NCPkgMenuAction.cc
  NCPkgMenuConfig.cc
  NCPkgMenuDeps.cc
  NCPkgMenuExtras.cc
  NCPkgMenuFilter.cc
  NCPkgMenuHelp.cc
  NCPkgMenuView.cc
  NCPkgPackageDetails.cc
  NCPkgPatchSearch.cc
  NCPkgPopupDeps.cc
  NCPkgPopupDescr.cc
  NCPkgPopupDiskspace.cc
  NCPkgPopupTable.cc
  NCPkgSearchSettings.cc
  NCPkgSelMapper.cc
  NCPkgStatusStrategy.cc
  NCPkgStrings.cc
  NCPkgTable.cc
  )


set( HEADERS
  NCPackageSelector.h
  NCPackageSelectorPluginImpl.h
  NCPackageSelectorStart.h
  
  NCPkgFilterClassification.h
  NCPkgFilterInstSummary.h
  NCPkgFilterLocale.h
  NCPkgFilterMain.h
  NCPkgFilterPattern.h
  NCPkgFilterRepo.h
  NCPkgFilterSearch.h
  NCPkgFilterService.h
  NCPkgMenuAction.h
  NCPkgMenuConfig.h
  NCPkgMenuDeps.h
  NCPkgMenuExtras.h
  NCPkgMenuFilter.h
  NCPkgMenuHelp.h
  NCPkgMenuView.h
  NCPkgPackageDetails.h
  NCPkgPatchSearch.h
  NCPkgPopupDeps.h
  NCPkgPopupDescr.h
  NCPkgPopupDiskspace.h
  NCPkgPopupTable.h
  NCPkgSearchSettings.h
  NCPkgSelMapper.h
  NCPkgStatusStrategy.h
  NCPkgStrings.h
  NCPkgTable.h
  NCZypp.h
  )



# Add shared lib to be built
add_library( ${TARGETLIB} SHARED ${SOURCES} ${HEADERS} )

# Include headers from the sibling sub-projects ../libyui and ../libyui-ncurses:
# We include headers as public, i.e.
#   #include <yui/YFoo.h>
#   #include <yui/ncurses/NCFoo.h>
#
# ../libyui/include has a symlink yui -> ../src which makes
# the libyui headers accessible as yui/YFoo.h;
# ../libyui-ncurses/include has a subdirectory yui/ with a symlink
# ncurses -> ../../src which makes the libyui-ncurses headers accessible as
# yui/ncurses/NCFoo.h .
target_include_directories( ${TARGETLIB} BEFORE PUBLIC ../../libyui/include )
target_include_directories( ${TARGETLIB} BEFORE PUBLIC ../../libyui-ncurses/include )

# Find yui and yui-ncurses during a combined build
target_link_directories( ${TARGETLIB} BEFORE PUBLIC ../../libyui/build/src )
target_link_directories( ${TARGETLIB} BEFORE PUBLIC ../../libyui-ncurses/build/src )

# Libraries that are needed to build this shared lib
target_link_libraries( ${TARGETLIB} yui yui-ncurses ${NCURSES_LIBS} zypp )

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
