# CMakeLists.txt for libyui-ncurses/pkgconfig
#
# Support for pkg-config:
#
# Generate a libyui-ncurses.pc file from libyui-ncurses.pc.in and install it to
# /usr/lib64/pkgconfig.
#
# A .pc file specifies how to use a development package, in particular linker
# flags (-lyui), compiler flags (including include directories) and paths.
# See man pkg-config.
#
# This .pc file is intended for extensions of the NCurses UI plug-in, not for
# applications using libyui.

include( ../../VERSION.cmake )
include( GNUInstallDirs )       # set CMAKE_INSTALL_LIBDIR


set( PKGCONFIG_INSTALL_DIR ${DESTDIR}${CMAKE_INSTALL_FULL_LIBDIR}/pkgconfig )

# Generate libyui-ncurses.pc where some CMake variables are expanded from libyui-ncurses.pc.in,
# but only expand @VARIABLE@, not ${VARIABLE}
configure_file( libyui-ncurses.pc.in libyui-ncurses.pc @ONLY )

install( FILES ${CMAKE_CURRENT_BINARY_DIR}/libyui-ncurses.pc DESTINATION ${PKGCONFIG_INSTALL_DIR} )
