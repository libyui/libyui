# CMakeLists.txt for libyui/pkgconfig
#
# Support for pkg-config:
#
# Generate a libyui.pc file from libyui.pc.in and install it to
# /usr/lib64/pkgconfig.
#
# A .pc file specifies how to use a development package, in particular linker
# flags (-lyui), compiler flags (including include directories) and paths.
# See man pkg-config.
#
# This .pc file is the only one used for applications that use the C++ API.

include( ../../VERSION.cmake )
include( GNUInstallDirs )       # set CMAKE_INSTALL_LIBDIR


set( PKGCONFIG_INSTALL_DIR ${DESTDIR}${CMAKE_INSTALL_FULL_LIBDIR}/pkgconfig )

# Generate libyui.pc where some CMake variables are expanded from libyui.pc.in,
# but only expand @VARIABLE@, not ${VARIABLE}
configure_file( libyui.pc.in libyui.pc @ONLY )

install( FILES ${CMAKE_CURRENT_BINARY_DIR}/libyui.pc DESTINATION ${PKGCONFIG_INSTALL_DIR} )
