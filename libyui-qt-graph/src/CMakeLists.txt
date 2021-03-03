# CMakeLists.txt for libyui-qt-pkg/src

include( ../../VERSION.cmake )
include( GNUInstallDirs )       # set CMAKE_INSTALL_INCLUDEDIR, ..._LIBDIR
include( FindPkgConfig )        # pkg_check_modules()

# Check for Qt
find_package( Qt5 COMPONENTS Widgets REQUIRED )
set( CMAKE_AUTOMOC on ) # Automatically handle "moc" preprocessor (Q_OBJECTs)

# Check for libgvc (from the libgraphviz package) via pkg-config;
# this sets LIBGVC_LIBRARIES (and more).
pkg_check_modules( LIBGVC REQUIRED libgvc )
message( "--   Graphviz libs: ${LIBGVC_LIBRARIES}" )


#
# libyui plugin specific
#

set( TARGETLIB          libyui-qt-graph )
set( TARGETLIB_BASE     yui-qt-graph    )

set( HEADERS_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR}/yui/qt-graph )
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
  YQGraph.cc
  YQGraphPlugin.cc
  YQGraphPluginImpl.cc
  QY2Graph.cc
  )


set( HEADERS
  YQGraph.h
  YQGraphPlugin.h
  YQGraphPluginImpl.h
  QY2Graph.h
  )



# Add shared lib to be built
add_library( ${TARGETLIB} SHARED
  ${SOURCES}
  ${HEADERS}
  )


#
# Include directories and compile options
#

set( LOCAL_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include )

# Create an include dir with some symlinks to include headers from sibling projects:
# build/src/include/yui/qt  -> ../../libyui-qt/src
# build/src/include/yui/yui -> ../../libyui/src
add_custom_target( local-include-dir
  COMMAND rm -rf   ${LOCAL_INCLUDE_DIR}
  COMMAND mkdir -p ${LOCAL_INCLUDE_DIR}/yui
  COMMAND ln -s ${CMAKE_CURRENT_SOURCE_DIR}/../../libyui/src    ${LOCAL_INCLUDE_DIR}/yui/yui
  COMMAND ln -s ${CMAKE_CURRENT_SOURCE_DIR}/../../libyui-qt/src ${LOCAL_INCLUDE_DIR}/yui/qt )

add_dependencies( ${TARGETLIB} local-include-dir )

# <yui/qt/YQFoo.h> from build/src/include/yui/qt -> ../../libyui-qt/src
target_include_directories( ${TARGETLIB} BEFORE PUBLIC ${LOCAL_INCLUDE_DIR} )

# <yui/YFoo.h> from build/src/include/yui/yui -> ../../libyui/src
target_include_directories( ${TARGETLIB} BEFORE PUBLIC ${LOCAL_INCLUDE_DIR}/yui )


# Make the version from ../../VERSION.cmake available as a #define
target_compile_definitions( ${TARGETLIB} PUBLIC VERSION="${VERSION}" )

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
  ${LIBGVC_LIBRARIES}
  Qt5::Core
  Qt5::Gui
  Qt5::Widgets
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
