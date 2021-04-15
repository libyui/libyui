# CMakeLists.txt for libyui-qt-graph/examples

include( ../../VERSION.cmake )
include( FindPkgConfig )        # pkg_check_modules()

set( PKG_NAME                   libyui-qt-graph${SONAME_MAJOR} )
set( EXAMPLES_INSTALL_DIR       ${DESTDIR}${CMAKE_INSTALL_PREFIX}/share/doc/packages/${PKG_NAME}/examples )


# Check for the libgvc (from the libgraphviz package) via pkg-config;
# this sets LIBGVC_LIBRARIES (and more).
pkg_check_modules( LIBGVC REQUIRED libgvc )


set( EXAMPLES_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include )

# Symlink ../../libyui/src to build/examples/include/yui
# so the headers there can be included as <yui/YFoo.h>
file( MAKE_DIRECTORY ${EXAMPLES_INCLUDE_DIR} )
file( CREATE_LINK ${CMAKE_CURRENT_SOURCE_DIR}/../../libyui/src ${EXAMPLES_INCLUDE_DIR}/yui SYMBOLIC )


# Build a simple example named EXAMPLE from one source file EXAMPLE.cc
# and install the source to the package's doc directory
#
function( add_example EXAMPLE )
  add_executable( ${EXAMPLE} ${EXAMPLE}.cc )
  target_link_libraries( ${EXAMPLE} yui ${LIBGVC_LIBRARIES} )
  add_dependencies( ${EXAMPLE} libyui-qt-graph )

  # Prefer the headers in ../../libyui/src over those in /usr/include
  target_include_directories( ${EXAMPLE} BEFORE PUBLIC ${EXAMPLES_INCLUDE_DIR} )

  install( FILES ${EXAMPLE}.cc DESTINATION ${EXAMPLES_INSTALL_DIR} )
endfunction()

#----------------------------------------------------------------------

add_example( SimpleCreate )
add_example( SimpleLoad   )
