cmake_minimum_required(VERSION 2.6)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0017 NEW)
endif(COMMAND cmake_policy)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})
SET(CMAKE_MODULE_PATH ${CMAKE_INSTALL_PREFIX}/share/cmake/Modules ${CMAKE_MODULE_PATH})

INCLUDE(${CMAKE_SOURCE_DIR}/VERSION.cmake)

SET( PACKAGE "libyui-bindings" )
SET( VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}" )

SET( LIBYUI_INCLUDE_DIR "/usr/include/yui" )
SET( LIBYUI_LIBRARY "yui" )

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Language Bindings for libyui")
SET(CPACK_PACKAGE_VENDOR "Novell Inc.")
#SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ReadMe.txt")
#SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/Copyright.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})

SET( CPACK_GENERATOR "TBZ2")
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
)

INCLUDE(CPack)

#FIND_PACKAGE(SWIG REQUIRED)
MESSAGE(STATUS "Looking modules in ${CMAKE_MODULE_PATH}")
FIND_PROGRAM(SWIG_EXECUTABLE
  NAMES swig-1.3 swig
  PATHS ${SWIG_DIR} ${SWIG_DIR}/.. ${SWIG_DIR}/../../bin /usr/bin /usr/local/bin ${CMAKE_INSTALL_PREFIX}/bin
)

IF ( NOT SWIG_EXECUTABLE )
  MESSAGE( FATAL "SWIG not found." )
ELSE ( NOT SWIG_EXECUTABLE )
  MESSAGE( STATUS "SWIG found at ${SWIG_EXECUTABLE}" )
ENDIF ( NOT SWIG_EXECUTABLE )

FIND_PACKAGE(Libyui REQUIRED)

ADD_SUBDIRECTORY(swig)

ADD_CUSTOM_TARGET( svncheck
  COMMAND cd $(CMAKE_SOURCE_DIR) && ! LC_ALL=C svn status --show-updates
  --quiet | grep -v '^Status against revision'
)

SET( SRCPACKAGE_COMMAND
  COMMAND ${CMAKE_MAKE_PROGRAM} package_source
  COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/package/${CPACK_SOURCE_PACKAGE_FILE_NAME}.tar.bz2
  COMMAND ${CMAKE_COMMAND} -E move ${CPACK_SOURCE_PACKAGE_FILE_NAME}.tar.bz2 ${CMAKE_BINARY_DIR}/package
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
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/libyui-bindings.spec.cmake ${CMAKE_BINARY_DIR}/package/libyui-bindings.spec @ONLY)
