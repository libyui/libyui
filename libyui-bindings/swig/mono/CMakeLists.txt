#
# cmake description for libyui CSharp bindings
#
# !!Attn!!: This creates two files
#  1. yui.so
#  2. yui.dll
# and the .dll file gets imported. See bindings/csharp/tests.
#

IF (NOT MONO_LIBRARIES)
   SET (MONO_LIBRARIES "/usr/lib/mono")
ENDIF (NOT MONO_LIBRARIES)

# SWIG_OUPUT is per-target
SET( SWIG_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/yui_csharp.cxx" )


ADD_CUSTOM_COMMAND (
   OUTPUT  ${SWIG_OUTPUT}
   COMMAND ${CMAKE_COMMAND} -E echo_append "Creating wrapper code for CSharp ..."
   COMMAND ${SWIG_EXECUTABLE} -c++ -csharp -features autodoc ${SWIG_FLAGS} -o ${SWIG_OUTPUT} -I${LIBYUI_INCLUDE_DIR} ${SWIG_INPUT}
   COMMAND ${CMAKE_COMMAND} -E echo "Done."
   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../*.i
)


SET(yui_csharp_SRCS ${SWIG_OUTPUT} )

ADD_LIBRARY( yui_csharp SHARED ${yui_csharp_SRCS} )
# name it 'yui' ==> i.e. yui.so
#  and don't prefix with 'lib'
SET_TARGET_PROPERTIES( yui_csharp PROPERTIES PREFIX "" OUTPUT_NAME "yui")

TARGET_LINK_LIBRARIES( yui_csharp ${LIBYUI_LIBRARY} )

#
# WORKAROUND: replace DllImport("yui" with DllImport("/abs_path/yui.so"
#
ADD_CUSTOM_COMMAND(
	TARGET yui_csharp
	POST_BUILD
	COMMAND sed -i -e 's|DllImport\(\"yui\"|DllImport\(\"${MONO_LIBRARIES}/yui/yui.so\"|g' "${CMAKE_CURRENT_BINARY_DIR}/yuiPINVOKE.cs" )


ADD_CUSTOM_COMMAND(
   TARGET yui_csharp POST_BUILD
   COMMAND "${CSHARP_MONO_COMPILER_${CSHARP_MONO_VERSION}}" /t:library *.cs /out:yui.dll
   COMMENT "Creating yui.dll"
   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

INSTALL(TARGETS yui_csharp LIBRARY DESTINATION "${MONO_LIBRARIES}/yui")

INSTALL(
   FILES ${CMAKE_CURRENT_BINARY_DIR}/yui.dll
   DESTINATION "${MONO_LIBRARIES}/yui"
   COMPONENT csharp
)
