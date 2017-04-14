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
	      
ADD_CUSTOM_COMMAND (
   OUTPUT  ${SWIG_OUTPUT}
   COMMAND ${CMAKE_COMMAND} -E echo_append "Creating wrapper code for Perl ..."
   COMMAND ${SWIG_EXECUTABLE} -c++ -perl -shadow -features autodoc -o ${SWIG_OUTPUT} -I${LIBYUI_INCLUDE_DIR} ${SWIG_INPUT}
   COMMAND ${CMAKE_COMMAND} -E echo "Done."
   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../*.i
)

SET(yui_perl_SRCS ${SWIG_OUTPUT} )

ADD_LIBRARY( yui_perl SHARED ${yui_perl_SRCS} )
# name it 'yui.so'
#  and don't prefix with 'lib'
SET_TARGET_PROPERTIES( yui_perl PROPERTIES PREFIX "" OUTPUT_NAME "yui")

INCLUDE_DIRECTORIES( ${PERL_CORE_DIR} ${LIBYUI_INCLUDE_DIR})

TARGET_LINK_LIBRARIES( yui_perl ${LIBYUI_LIBRARY} )
TARGET_LINK_LIBRARIES( yui_perl ${PERL_LIBRARY} )

INSTALL(TARGETS yui_perl LIBRARY DESTINATION ${PERL_VENDOR_ARCH} )

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/yui.pm DESTINATION ${PERL_VENDOR_LIB} )
