#
# cmake description for libyui CSharp bindings
#
# !!Attn!!: This creates two files
#  1. yui.so
#  2. yui.cs
# and the .cs file gets imported. See bindings/csharp/tests.
#

# SWIG_OUPUT is per-target
SET( SWIG_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/yui_csharp.cxx" )


MESSAGE(STATUS "CSharp executable: ${MONO_EXECUTABLE}")
MESSAGE(STATUS "CSharp include dir: ${MONO_PATH}")

ADD_CUSTOM_COMMAND (
   OUTPUT  ${SWIG_OUTPUT}
   COMMAND ${CMAKE_COMMAND} -E echo_append "Creating wrapper code for CSharp ..."
   COMMAND ${SWIG_EXECUTABLE} -c++ -csharp -features autodoc -o ${SWIG_OUTPUT} -I${LIBYUI_INCLUDE_DIR} ${SWIG_INPUT}
   COMMAND ${CMAKE_COMMAND} -E echo "Done."
   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../*.i
)

SET(yui_csharp_SRCS ${SWIG_OUTPUT} )

ADD_LIBRARY( yui_csharp SHARED ${yui_csharp_SRCS} )
# name it 'yui' ==> i.e. yui.so
#  and don't prefix with 'lib'
# SET_TARGET_PROPERTIES( yui_csharp PROPERTIES PREFIX "" OUTPUT_NAME "_yui")
SET_TARGET_PROPERTIES( yui_csharp PROPERTIES PREFIX "" OUTPUT_NAME "yui")

INCLUDE_DIRECTORIES( ${LIBYUI_INCLUDE_DIR} )

TARGET_LINK_LIBRARIES( yui_csharp ${LIBYUI_LIBRARY} )

INSTALL(TARGETS yui_csharp LIBRARY DESTINATION "${MONO_LIBRARIES}/yui")

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/yui.cs DESTINATION "${MONO_LIBRARIES}/yui" )
