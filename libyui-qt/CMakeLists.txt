# CMakeLists.txt for libyui-qt
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
project( libyui-qt )

# Options usage:
#
#   cmake -DBUILD_DOC=on -DBUILD_EXAMPLES=off ..

option( BUILD_SRC         "Build in src/ subdirectory"                on )
option( BUILD_DOC         "Build class documentation"                 off )
option( WERROR            "Treat all compiler warnings as errors"     on  )


#----------------------------------------------------------------------


# Use the $PREFIX environment variable to change the default installation prefix:
#
#   PREFIX=/usr/local cmake ..

if ( DEFINED ENV{PREFIX} )
  set( CMAKE_INSTALL_PREFIX $ENV{PREFIX} )
else()
  set( CMAKE_INSTALL_PREFIX /usr )
endif()

message( "-- CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}" )


set( CMAKE_INSTALL_MESSAGE LAZY ) # Suppress "up-to-date" messages during "make install"


# Initialize compiler flags for all targets in all subdirectories
add_compile_options( "-Wall" )
add_compile_options( "-Os" )    # Optimize for size (overrides CMake's -O3 in RELEASE builds)

if ( WERROR )
  add_compile_options( "-Werror" )
endif()


#
# Descend into subdirectories
#

if ( BUILD_SRC )
  add_subdirectory( src )
endif()

if ( BUILD_DOC )
  add_subdirectory( doc )
endif()
