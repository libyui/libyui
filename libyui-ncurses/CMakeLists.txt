# CMakeLists.txt for libyui-ncurses
#
# Usage:
#
#   mkdir build
#   cd build
#   cmake ..
#
#   make
#   sudo make install
#
# Restart with a clean build environment:
#   rm -rf build
#
# Show the complete compiler commands with all arguments:
#   make VERBOSE=1

cmake_minimum_required( VERSION 3.10 )
project( libyui-ncurses )

# Options usage:
#
#   cmake -DBUILD_DOC=on -DBUILD_EXAMPLES=off ..

option( BUILD_SRC         "Build in src/ subdirectory"                on )
option( BUILD_DOC         "Build class documentation"                 off )
option( BUILD_PKGCONFIG   "Build pkg-config support files"            on  )
option( WERROR            "Treat all compiler warnings as errors"     on  )


#----------------------------------------------------------------------


# We use /usr as the default CMAKE_INSTALL_PREFIX, but it can be set on the
# cmake command line with
#
#   cmake -DCMAKE_INSTALL_PREFIX=/my/install/prefix ..
#
# or in the environment with
#
#   CMAKE_INSTALL_PREFIX=/usr/local  cmake ..

if ( CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
  if ( DEFINED ENV{CMAKE_INSTALL_PREFIX} )
    set( CIP $ENV{CMAKE_INSTALL_PREFIX} )
  else()
    set( CIP /usr )
  endif()
  set( CMAKE_INSTALL_PREFIX "${CIP}" CACHE PATH "Install path prefix" FORCE)
endif()

message( "-- CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}" )


set( CMAKE_INSTALL_MESSAGE LAZY ) # Suppress "up-to-date" messages during "make install"


# Initialize compiler flags for all targets in all subdirectories
add_compile_options( "-Wall" )
add_compile_options( "-Os" )    # Optimize for size (overrides CMake's -O3 in RELEASE builds)
add_compile_options( "-D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64" ) # Enable largefile support
if ( WERROR )
  add_compile_options( "-Werror" )
endif()


#
# Descend into subdirectories
#

enable_testing()
add_subdirectory( tests )

# Build and install auxiliary files first so this doesn't scroll away any important messages

if ( BUILD_PKGCONFIG )
  add_subdirectory( pkgconfig )
endif()

#----------------------------------------------------------------------

if ( BUILD_SRC )
  add_subdirectory( src )
endif()

if ( BUILD_DOC )
  add_subdirectory( doc )
endif()
