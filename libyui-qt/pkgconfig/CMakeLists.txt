# CMakeLists.txt for libyui-qt/pkgconfig
#
# Support for pkg-config:
#
# Generate a libyui-qt.pc file from libyui-qt.pc.in and install it to
# /usr/lib64/pkgconfig.
#
# A .pc file specifies how to use a development package, in particular linker
# flags (-lyui), compiler flags (including include directories) and paths.
# See man pkg-config.
#
# This .pc file is intended for extensions of the Qt UI plug-in, not for
# applications using libyui.

include( ../../VERSION.cmake )
include( GNUInstallDirs )       # set CMAKE_INSTALL_LIBDIR


set( PKGCONFIG_INSTALL_DIR ${DESTDIR}${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/pkgconfig )

# Generate libyui-qt.pc where some CMake variables are expanded from libyui-qt.pc.in,
# but only expand @VARIABLE@, not ${VARIABLE}
configure_file( libyui-qt.pc.in libyui-qt.pc @ONLY )

install( FILES ${CMAKE_CURRENT_BINARY_DIR}/libyui-qt.pc DESTINATION ${PKGCONFIG_INSTALL_DIR} )
