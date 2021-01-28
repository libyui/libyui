# CMakeLists.txt for libyui-qt-graph/examples

include( ../VERSION.cmake )
include( FindPkgConfig )        # pkg_check_modules()

set( PKG_NAME                   libyui-qt-graph${SONAME_MAJOR} )
set( EXAMPLES_INSTALL_DIR       ${DESTDIR}/usr/share/doc/packages/${PKG_NAME}/examples )


# Check for the libgvc (from the libgraphviz package) via pkg-config;
# this sets LIBGVC_LIBRARIES (and more).
pkg_check_modules( LIBGVC REQUIRED libgvc )


set( EXAMPLES_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include )

# Symlink ../../libyui/src to build/examples/include/yui
# so the headers there can be included as <yui/YFoo.h>
add_custom_target( examples-include-dir
  COMMAND rm -rf ${EXAMPLES_INCLUDE_DIR}
  COMMAND mkdir  ${EXAMPLES_INCLUDE_DIR}
  COMMAND ln -s  ${CMAKE_CURRENT_SOURCE_DIR}/../../libyui/src ${EXAMPLES_INCLUDE_DIR}/yui )


# Build a simple example named EXAMPLE from one source file EXAMPLE.cc
# and install the source to the package's doc directory
#
function( add_example EXAMPLE )
  add_executable( ${EXAMPLE} ${EXAMPLE}.cc )
  target_link_libraries( ${EXAMPLE} yui ${LIBGVC_LIBRARIES} )
  add_dependencies( ${EXAMPLE} libyui-qt-graph examples-include-dir )

  # Prefer the headers in ../../libyui/src over those in /usr/include
  target_include_directories( ${EXAMPLE} BEFORE PUBLIC ${EXAMPLES_INCLUDE_DIR} )

  install( FILES ${EXAMPLE}.cc DESTINATION ${EXAMPLES_INSTALL_DIR} )
endfunction()

#----------------------------------------------------------------------

add_example( SimpleCreate )
add_example( SimpleLoad   )
