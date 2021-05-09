#
# cmake description for libyui Perl bindings
#
# !!Attn!!: This creates two files
#  1. _yui.so
#  2. yui.pm
# and the .pm file gets loaded. See bindings/perl/tests.
#

# SWIG_OUPUT is per-target
SET( SWIG_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/yui_perl.cxx" )

EXECUTE_PROCESS(COMMAND ${PERL_EXECUTABLE} -e "use Config; print \$Config{ccflags}" OUTPUT_VARIABLE PERL_CCFLAGS)
EXECUTE_PROCESS(COMMAND ${PERL_EXECUTABLE} -e "use Config; print \$Config{archlib}.\"/CORE\"" OUTPUT_VARIABLE PERL_CORE_DIR)
EXECUTE_PROCESS(COMMAND ${PERL_EXECUTABLE} -e "use Config; print \$Config{installvendorarch}" OUTPUT_VARIABLE PERL_VENDOR_ARCH)
EXECUTE_PROCESS(COMMAND ${PERL_EXECUTABLE} -e "use Config; print \$Config{installvendorlib}" OUTPUT_VARIABLE PERL_VENDOR_LIB)
EXECUTE_PROCESS(COMMAND ${PERL_EXECUTABLE} -e "use Config; print \$Config{ccdlflags}; print \",-lpthread\"" OUTPUT_VARIABLE PERL_LINK_FLAGS)

STRING(REGEX REPLACE "^/usr/" "${CMAKE_INSTALL_PREFIX}/" PERL_VENDOR_ARCH "${PERL_VENDOR_ARCH}")
STRING(REGEX REPLACE "^/usr/" "${CMAKE_INSTALL_PREFIX}/" PERL_VENDOR_LIB "${PERL_VENDOR_LIB}")

MESSAGE(STATUS "Perl executable: ${PERL_EXECUTABLE}")
MESSAGE(STATUS "Perl core dir: ${PERL_CORE_DIR}")
MESSAGE(STATUS "Perl vendor arch dir: ${PERL_VENDOR_ARCH}")
MESSAGE(STATUS "Perl vendor lib dir: ${PERL_VENDOR_LIB}")

ADD_DEFINITIONS( ${PERL_CCFLAGS} -Wno-unused -Wno-error )
LINK_DIRECTORIES( ${PERL_CORE_DIR} )

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

set( LOCAL_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include )

if(NOT WITH_MGA)
  # Symlink ../../../libyui/src to build/src/include/yui
  # so the headers there can be included as <yui/YFoo.h>
  file( MAKE_DIRECTORY ${LOCAL_INCLUDE_DIR} )
  file( CREATE_LINK ${CMAKE_CURRENT_SOURCE_DIR}/../../../libyui/src ${LOCAL_INCLUDE_DIR}/yui SYMBOLIC )
endif()

ADD_CUSTOM_COMMAND (
   OUTPUT  ${SWIG_OUTPUT}
   COMMAND ${CMAKE_COMMAND} -E echo_append "Creating wrapper code for Perl ..."
   COMMAND ${SWIG_EXECUTABLE} -c++ -perl -shadow -features autodoc ${SWIG_FLAGS} -o ${SWIG_OUTPUT} -I${LOCAL_INCLUDE_DIR} ${LIBYUI_INCLUDE_CFLAGS} ${SWIG_INPUT}
   COMMAND ${CMAKE_COMMAND} -E echo "Done."
   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../*.i
)

SET(yui_perl_SRCS ${SWIG_OUTPUT} )

set( TARGETLIB yui_perl )
ADD_LIBRARY( ${TARGETLIB} SHARED ${yui_perl_SRCS} )
# name it 'yui.so'
#  and don't prefix with 'lib'
SET_TARGET_PROPERTIES( ${TARGETLIB} PROPERTIES PREFIX "" OUTPUT_NAME "yui")

target_include_directories( ${TARGETLIB} BEFORE PUBLIC ${LOCAL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${PERL_CORE_DIR} ${LIBYUI_INCLUDE_DIR})

if (NOT WITH_MGA)
  # Find yui during a combined build
  target_link_directories( ${TARGETLIB} BEFORE PUBLIC ../../../libyui/build/src )
else()
  target_link_directories( ${TARGETLIB}
    PUBLIC ${YUI_LIBRARY_DIRS}
    PUBLIC ${YUIMGA_LIBRARY_DIRS}
  )
endif()

TARGET_LINK_LIBRARIES( ${TARGETLIB} ${LIBYUI_LIBRARY} )
TARGET_LINK_LIBRARIES( ${TARGETLIB} ${PERL_LIBRARY} )

INSTALL(TARGETS ${TARGETLIB} LIBRARY DESTINATION ${PERL_VENDOR_ARCH} )

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/yui.pm DESTINATION ${PERL_VENDOR_LIB} )
