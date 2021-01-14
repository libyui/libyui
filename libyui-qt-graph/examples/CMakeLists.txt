# CMakeLists.txt for libyui-qt-graph/examples

include( ../VERSION.cmake )
include( FindPkgConfig )        # pkg_check_modules()

set( PKG_NAME                   libyui-qt-graph${SONAME_MAJOR} )
set( EXAMPLES_INSTALL_DIR       ${DESTDIR}/usr/share/doc/packages/${PKG_NAME}/examples )

set( USE_FRESHLY_BUILT_LIBYUI   off )

if ( USE_FRESHLY_BUILT_LIBYUI )
  # Use the freshly built libyui from ../build/src, not the system libyui from
  # /usr/lib64; also set rpath.
  #
  # FIXME: This will still load the UI plug-ins from the system, not the
  # freshly built ones.
  set( LIBYUI ${CMAKE_BINARY_DIR}/src/libyui.so )
else()
  # Link against the system libyui in /usr/lib64. No rpath.
  set( LIBYUI yui )
endif()

# Check for the libgvc (from the libgraphviz package) via pkg-config;
# this sets LIBGVC_LIBRARIES (and more).
pkg_check_modules( LIBGVC REQUIRED libgvc )


# Build a simple example named EXAMPLE from one source file EXAMPLE.cc
# and install the source to the package's doc directory
#
function( add_example EXAMPLE )
  add_executable( ${EXAMPLE} ${EXAMPLE}.cc )
  target_link_libraries( ${EXAMPLE} ${LIBYUI} ${LIBGVC_LIBRARIES} )

  # Add a dependency to the libyui-qt-graph CMake target so ../src/ is built first
  add_dependencies( ${EXAMPLE} libyui-qt-graph )

  if ( USE_FRESHLY_BUILT_LIBYUI )
    # Enable including headers like <yui/YFoo.h> from ../../libyui/include
    # which has a symlink yui -> ../src
    target_include_directories( ${EXAMPLE} BEFORE PUBLIC ../../libyui/include )
  endif()

  install( FILES ${EXAMPLE}.cc DESTINATION ${EXAMPLES_INSTALL_DIR} )
endfunction()

#----------------------------------------------------------------------

add_example( SimpleCreate )
add_example( SimpleLoad   )
