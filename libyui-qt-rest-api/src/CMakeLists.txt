# CMakeLists.txt for libyui-qt-rest-api/src

include( ../VERSION.cmake )
include( GNUInstallDirs )       # set CMAKE_INSTALL_INCLUDEDIR, ..._LIBDIR

#
# Qt-specific
#

find_package( Qt5 COMPONENTS Widgets Svg X11Extras REQUIRED )

set( CMAKE_AUTOMOC on ) # Automatically handle "moc" preprocessor (Q_OBJECTs)


#
# libyui-rest-api plugin specific
#

set( TARGETLIB          libyui-qt-rest-api )
set( TARGETLIB_BASE     yui-qt-rest-api    )

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
  YQHttpUI.cc
  YQHttpWidgetsActionHandler.cc
  YQTableActionHandler.cc
  YQWidgetActionHandler.cc
  )


set( HEADERS
  YQHttpUI.h
  YQHttpWidgetsActionHandler.h
  YQTableActionHandler.h
  YQWidgetActionHandler.h
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
# ../libyui-rest-api, ../libyui-qt:
# We include headers as public, i.e.
#   #include <yui/YFoo.h>
#   #include <yui/rest-api/YRestFoo.h>
#   #include <yui/qt/YQFoo.h>
#
# Each ../libyui*/include has a symlink yui -> ../src which makes
# the libyui headers accessible as yui/YFoo.h etc.
target_include_directories( ${TARGETLIB} BEFORE PUBLIC ../../libyui/include )
target_include_directories( ${TARGETLIB} BEFORE PUBLIC ../../libyui-rest-api/include )
target_include_directories( ${TARGETLIB} BEFORE PUBLIC ../../libyui-qt/include )

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

# libyui-*.so* are in /usr/lib64/yui, not in /usr/lib64
target_link_directories( ${TARGETLIB} BEFORE PUBLIC /usr/${CMAKE_INSTALL_LIBDIR}/yui )

# Find libs in a sibling directory when building from a single tarball
target_link_directories( ${TARGETLIB} BEFORE PUBLIC ../../libyui/build/src )
target_link_directories( ${TARGETLIB} BEFORE PUBLIC ../../libyui-rest-api/build/src )
target_link_directories( ${TARGETLIB} BEFORE PUBLIC ../../libyui-qt/build/src )


# Libraries that are needed to build this shared lib
target_link_libraries( ${TARGETLIB}
  yui-qt
  yui-rest-api
  yui
  Qt5::Core
  Qt5::Gui
  Qt5::Widgets
  Qt5::Svg
  Qt5::X11Extras
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
