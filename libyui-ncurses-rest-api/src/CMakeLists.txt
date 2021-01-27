# CMakeLists.txt for libyui-ncurses-rest-api/src

include( ../VERSION.cmake )
include( GNUInstallDirs )       # set CMAKE_INSTALL_INCLUDEDIR, ..._LIBDIR


#
# libyui-rest-api plugin specific
#

set( TARGETLIB          libyui-ncurses-rest-api )
set( TARGETLIB_BASE     yui-ncurses-rest-api    )

set( HEADERS_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR}/yui/ncurses-rest-api )
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
  YNCHttpUI.cc
  YNCHttpWidgetsActionHandler.cc
  YNCWidgetActionHandler.cc
  NCHttpWidgetFactory.cc
  NCHttpDialog.cc
  )


set( HEADERS
  YNCHttpUI.h
  YNCHttpWidgetsActionHandler.h
  YNCWidgetActionHandler.h
  NCHttpWidgetFactory.h
  NCHttpDialog.h
  )


# Add shared lib to be built
add_library( ${TARGETLIB} SHARED
  ${SOURCES}
  ${HEADERS}
  )


#
# Include directories and compile options
#

# Include headers from the sibling sub-projects ../libyui,
# ../libyui-rest-api, ../libyui-ncurses:
# We include headers as public, i.e.
#   #include <yui/YFoo.h>
#   #include <yui/rest-api/YRestFoo.h>
#   #include <yui/ncurses/NCFoo.h>
#
# Each ../libyui*/include has a symlink yui -> ../src which makes
# the libyui headers accessible as yui/YFoo.h etc.
target_include_directories( ${TARGETLIB} BEFORE PUBLIC ../../libyui/include )
target_include_directories( ${TARGETLIB} BEFORE PUBLIC ../../libyui-rest-api/include )
target_include_directories( ${TARGETLIB} BEFORE PUBLIC ../../libyui-ncurses/include )

# FIXME start
# Also add those directories directly until the full transition is done:
# The headers of older libyui-rest-api packages still #include "YSomeWidget.h"
target_include_directories( ${TARGETLIB} BEFORE PUBLIC /usr/include/yui )
# Remove this when those old packages are history.
# FIXME end

# Make the version from ../VERSION.cmake available as a #define
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
