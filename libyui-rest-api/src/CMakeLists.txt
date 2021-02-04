# CMakeLists.txt for libyui-rest-api/src

include( ../VERSION.cmake )
include( GNUInstallDirs )       # set CMAKE_INSTALL_INCLUDEDIR, ..._LIBDIR

# Check if the libs we link against are available

find_package( Boost REQUIRED ) # pkg boost-devel
find_library( JSONCPP_LIB    NAMES jsoncpp    REQUIRED ) # pkg jsoncpp-devel
find_library( MICROHTTPD_LIB NAMES microhttpd REQUIRED ) # pkg libmicrohttpd-devel

message( "-- jsoncpp lib: ${JSONCPP_LIB}" )
message( "-- microhttpd lib: ${MICROHTTPD_LIB}" )


set( TARGETLIB          libyui-rest-api )
set( TARGETLIB_BASE     yui-rest-api    )

set( HEADERS_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR}/yui/rest-api )
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
 YHttpServer.cc
 YHttpAppHandler.cc
 YHttpDialogHandler.cc
 YHttpHandler.cc
 YHttpMount.cc
 YHttpRootHandler.cc
 YHttpVersionHandler.cc
 YHttpWidgetsActionHandler.cc
 YHttpWidgetsHandler.cc

 YJsonSerializer.cc
 YTableActionHandler.cc
 YWidgetFinder.cc
 )


set( HEADERS
 YHttpServer.h
 YHttpServerSockets.h

 YHttpAppHandler.h
 YHttpDialogHandler.h
 YHttpHandler.h
 YHttpMount.h
 YHttpRootHandler.h
 YHttpVersionHandler.h
 YHttpWidgetsActionHandler.h
 YHttpWidgetsHandler.h

 YJsonSerializer.h
 YTableActionHandler.h
 YWidgetActionHandler.h
 YWidgetFinder.h
 )


# Add shared lib to be built
add_library( ${TARGETLIB} SHARED ${SOURCES} ${HEADERS} )


#
# Include directories and compile options
#

set( LOCAL_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include )

# Symlink ../../libyui/src to build/src/include/yui
# so the headers there can be included as <yui/YFoo.h>
add_custom_target( local-include-dir
  COMMAND rm -rf ${LOCAL_INCLUDE_DIR}
  COMMAND mkdir  ${LOCAL_INCLUDE_DIR}
  COMMAND ln -s  ${CMAKE_CURRENT_SOURCE_DIR}/../../libyui/src ${LOCAL_INCLUDE_DIR}/yui )

add_dependencies( ${TARGETLIB} local-include-dir )
target_include_directories( ${TARGETLIB} BEFORE PUBLIC ${LOCAL_INCLUDE_DIR} )


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
# We only use the parts of Boost that are completely contained in header files
# without any binary part, so we don't need to link against any Boost lib.
# Should that become necessary, add Boost::filesystem or whatever lib is
# needed. See also
# https://cliutils.gitlab.io/modern-cmake/chapters/packages/Boost.html
#
# If in doubt what is really needed, check with "ldd -u" which libs are unused.
target_link_libraries( ${TARGETLIB}
  yui
  ${JSONCPP_LIB}
  ${MICROHTTPD_LIB}
  )


# https://cmake.org/cmake/help/latest/manual/cmake-properties.7.html#target-properties
set_target_properties( ${TARGETLIB} PROPERTIES
  VERSION       ${SONAME}           # From ../VERSION.cmake
  SOVERSION     ${SONAME_MAJOR}     # From ../VERSION.cmake
  OUTPUT_NAME   ${TARGETLIB_BASE}
  )


#
# Install
#

# Install the headers first so the message about the lib does not scroll away
install( FILES   ${HEADERS}   DESTINATION ${HEADERS_INSTALL_DIR} )
install( TARGETS ${TARGETLIB} LIBRARY DESTINATION ${PLUGIN_DIR} )
