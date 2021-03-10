# CMakeLists.txt for libyui-ncurses/src

include( ../../VERSION.cmake )
include( GNUInstallDirs )       # set CMAKE_INSTALL_INCLUDEDIR, ..._LIBDIR


# Check if the libs we link against are available.
# They are all part of package ncurses-devel.

find_library( TINFO_LIB    NAMES tinfo    REQUIRED )
# We don't use libncursesw or libpanelw here. Check with  ldd -u .

set( NCURSES_LIBS ${PANELW_LIB} ${TINFO_LIB} )

# find_library( zypp ) is pointless because there is a libzypp on every SUSE


#
# libyui plugin specific
#

set( TARGETLIB          libyui-ncurses-pkg )
set( TARGETLIB_BASE     yui-ncurses-pkg    )

set( HEADERS_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR}/yui/ncurses-pkg )
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

#
# Include directories and compile options
#

set( LOCAL_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include )

# Create an include dir with some symlinks to include headers from sibling projects:
# build/src/include/yui/ncurses -> ../../libyui-ncurses/src
# build/src/include/yui/yui     -> ../../libyui/src
add_custom_target( local-include-dir
  COMMAND rm -rf   ${LOCAL_INCLUDE_DIR}
  COMMAND mkdir -p ${LOCAL_INCLUDE_DIR}/yui
  COMMAND ln -s ${CMAKE_CURRENT_SOURCE_DIR}/../../libyui/src ${LOCAL_INCLUDE_DIR}/yui/yui
  COMMAND ln -s ${CMAKE_CURRENT_SOURCE_DIR}/../../libyui-ncurses/src ${LOCAL_INCLUDE_DIR}/yui/ncurses )

add_dependencies( ${TARGETLIB} local-include-dir )

# <yui/ncurses/NCFoo.h> from build/src/include/yui/ncurses -> ../../libyui-ncurses/src
target_include_directories( ${TARGETLIB} BEFORE PUBLIC ${LOCAL_INCLUDE_DIR} )

# <yui/YFoo.h> from build/src/include/yui/yui -> ../../libyui/src
target_include_directories( ${TARGETLIB} BEFORE PUBLIC ${LOCAL_INCLUDE_DIR}/yui )


# Workaround for boost::bind() complaining about deprecated _1 placeholder
# deep in the libzypp headers
target_compile_definitions( ${TARGETLIB} PUBLIC BOOST_BIND_GLOBAL_PLACEHOLDERS=1 )

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


#
# Linking
#

# Find yui during a combined build
target_link_directories( ${TARGETLIB} BEFORE PUBLIC ../../libyui/build/src )


# Libraries that are needed to build this shared lib
#
# If in doubt what is really needed, check with "ldd -u" which libs are unused.
target_link_libraries( ${TARGETLIB}
  yui
  zypp
  ${NCURSES_LIBS}
  )


# https://cmake.org/cmake/help/latest/manual/cmake-properties.7.html#target-properties
set_target_properties( ${TARGETLIB} PROPERTIES
  VERSION       ${SONAME}           # From ../../VERSION.cmake
  SOVERSION     ${SONAME_MAJOR}     # From ../../VERSION.cmake
  OUTPUT_NAME   ${TARGETLIB_BASE}
  )


#
# Install
#

# Install the headers first so the message about the lib does not scroll away
install( FILES   ${HEADERS}   DESTINATION ${HEADERS_INSTALL_DIR} )
install( TARGETS ${TARGETLIB} LIBRARY DESTINATION ${PLUGIN_DIR} )
