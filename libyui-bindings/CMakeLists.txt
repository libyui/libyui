#
# CMakeLists.txt for libyui-bindings
#

cmake_minimum_required(VERSION 2.6)
project( libyui-bindings )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0017 NEW)
  if(POLICY CMP0046)
    cmake_policy(SET CMP0046 OLD)
  endif()
endif(COMMAND cmake_policy)

OPTION( BUILD_RUBY_GEM "Build Ruby GEM" YES )
OPTION( WITH_MGA "Build with support for libyui-mga" NO )


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


# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules ${CMAKE_MODULE_PATH})
SET(CMAKE_MODULE_PATH ${CMAKE_INSTALL_PREFIX}/share/cmake/Modules ${CMAKE_MODULE_PATH})

INCLUDE(../VERSION.cmake)

SET( PACKAGE "libyui-bindings" )
SET( VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}" )

IF( NOT YPREFIX )
  SET( YPREFIX "/usr" )
ENDIF()


SET( LIBYUI_INCLUDE_DIR "${YPREFIX}/include" )

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Language Bindings for libyui")
SET(CPACK_PACKAGE_VENDOR "Novell Inc.")
#SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ReadMe.txt")
#SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/Copyright.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})

if (GENERATE)
    SET(CPACK_PACKAGE_CONTACT "SUSE")
    # rpm
    SET(CPACK_RPM_PACKAGE_LICENSE LGPL)
    SET(CPACK_RPM_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${VERSION_PATCH}")
    SET(CPACK_RPM_PACKAGE_RELEASE 1)

    SET(CPACK_PACKAGE_FILE_NAME "${PACKAGE}-${CPACK_RPM_PACKAGE_VERSION}-${CPACK_RPM_PACKAGE_RELEASE}")
    # deb
    SET(CPACK_DEBIAN_PACKAGE_NAME "${CPACK_PACKAGE_FILE_NAME}")
    SET(CPACK_DEBIAN_PACKAGE_VERSION "${CPACK_RPM_PACKAGE_VERSION}-${CPACK_RPM_PACKAGE_RELEASE}")
    SET(CPACK_GENERATOR ${GENERATE})
else (GENERATE)
    # tbz2
    SET(CPACK_PACKAGE_FILE_NAME "${PACKAGE}-bin-${PACKAGE_FILE_VERSION}")
    SET( CPACK_GENERATOR "TBZ2" )
endif (GENERATE)

SET( CPACK_SOURCE_GENERATOR "TBZ2")
SET( CPACK_SOURCE_PACKAGE_FILE_NAME "${PACKAGE}-${VERSION}" )

# The following components are regex's to match anywhere (unless anchored)
# in absolute path + filename to find files or directories to be excluded
# from source tarball.
SET (CPACK_SOURCE_IGNORE_FILES
"/CVS/;/.svn/;/.libs/;/.deps/;.swp$;.#;/#;/build/"
"~$"
"\\\\.cvsignore$"
"/package"
"Makefile\\\\.in$"
"/\\\\.git/"
"\\\\.gitignore$"
)

INCLUDE(CPack)

MESSAGE(STATUS "Looking for modules in ${CMAKE_MODULE_PATH}")

FIND_PACKAGE(SWIG REQUIRED)

FIND_PACKAGE(PkgConfig REQUIRED)

# Don't require libyui just because it is always built into source tree
PKG_CHECK_MODULES(YUI libyui)
if (YUI_FOUND)
  pkg_get_variable(YUI_INCLUDE_DIR libyui includedir)
  message (STATUS "Found libyui into ${YUI_LIBRARY_DIRS}")
  SET( LIBYUI_LIBRARY ${YUI_LIBRARIES} )
  SET( LIBYUI_INCLUDE_DIR ${YUI_INCLUDE_DIR} )
  SET( LIBYUI_INCLUDE_CFLAGS -I${YUI_INCLUDE_DIR} )
else()
  message (STATUS "System libyui not found")
  # set them anyway for subdir CMakeLists.txt
  SET( LIBYUI_LIBRARY yui )
  SET( LIBYUI_INCLUDE_DIR /usr/include )
  SET( LIBYUI_INCLUDE_CFLAGS -I${YUI_INCLUDE_DIR} )
endif()

IF(WITH_MGA)
  # Don't require libyui-mga just because it could be built into libyui source tree
  PKG_CHECK_MODULES(YUIMGA libyui-mga)
  if (YUIMGA_FOUND)
    pkg_get_variable(YUIMGA_INCLUDE_DIR libyui-mga includedir)
    message (STATUS "Found libyui-mga into ${YUIMGA_LIBRARY_DIRS}")

    SET( LIBYUI_LIBRARY ${LIBYUI_LIBRARY} ${YUIMGA_LIBRARIES} )
    SET( LIBYUI_INCLUDE_DIR ${LIBYUI_INCLUDE_DIR} ${YUIMGA_INCLUDE_DIR})
    SET( LIBYUI_INCLUDE_CFLAGS ${LIBYUI_INCLUDE_CFLAGS} -I${YUIMGA_INCLUDE_DIR} )
  else()
    # set them anyway for subdir CMakeLists.txt
    SET( LIBYUI_LIBRARY ${LIBYUI_LIBRARY} yui-mga )
  endif()

  ADD_DEFINITIONS( -DWITH_MGA )
  SET( SWIG_FLAGS "-DWITH_MGA" )
ENDIF(WITH_MGA)

ADD_SUBDIRECTORY(swig)

ADD_CUSTOM_TARGET( svncheck
  COMMAND cd $(CMAKE_SOURCE_DIR) && ! LC_ALL=C svn status --show-updates
  --quiet | grep -v '^Status against revision'
)

SET( SRCPACKAGE_COMMAND
  COMMAND ${CMAKE_MAKE_PROGRAM} package_source
  COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/package/${CPACK_SOURCE_PACKAGE_FILE_NAME}.tar.bz2
  COMMAND ${CMAKE_COMMAND} -E copy ${CPACK_SOURCE_PACKAGE_FILE_NAME}.tar.bz2 ${CMAKE_BINARY_DIR}/package
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/package/${PACKAGE}.changes ${CMAKE_BINARY_DIR}/package
)

ADD_CUSTOM_TARGET( srcpackage_local
  ${SRCPACKAGE_COMMAND}
)

ADD_CUSTOM_TARGET( srcpackage
  COMMAND ${CMAKE_MAKE_PROGRAM} svncheck
  ${SRCPACKAGE_COMMAND}
)

MESSAGE(STATUS "Writing spec file...")
