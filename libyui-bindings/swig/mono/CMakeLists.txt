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


set( LOCAL_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include )

if(NOT WITH_MGA)
  # Symlink ../../../libyui/src to build/src/include/yui
  # so the headers there can be included as <yui/YFoo.h>
  file( MAKE_DIRECTORY ${LOCAL_INCLUDE_DIR} )
  file( CREATE_LINK ${CMAKE_CURRENT_SOURCE_DIR}/../../../libyui/src ${LOCAL_INCLUDE_DIR}/yui SYMBOLIC )
endif()

ADD_CUSTOM_COMMAND (
   OUTPUT  ${SWIG_OUTPUT}
   COMMAND ${CMAKE_COMMAND} -E echo_append "Creating wrapper code for CSharp ..."
   COMMAND ${SWIG_EXECUTABLE} -c++ -csharp -features autodoc ${SWIG_FLAGS} -o ${SWIG_OUTPUT} -I${LOCAL_INCLUDE_DIR} ${LIBYUI_INCLUDE_CFLAGS} ${SWIG_INPUT}
   COMMAND ${CMAKE_COMMAND} -E echo "Done."
   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../*.i
)


SET(yui_csharp_SRCS ${SWIG_OUTPUT} )

set( TARGETLIB yui_csharp )
ADD_LIBRARY( ${TARGETLIB} SHARED ${yui_csharp_SRCS} )
# name it 'yui' ==> i.e. yui.so
#  and don't prefix with 'lib'
SET_TARGET_PROPERTIES( ${TARGETLIB} PROPERTIES PREFIX "" OUTPUT_NAME "yui")

target_include_directories( ${TARGETLIB} BEFORE PUBLIC ${LOCAL_INCLUDE_DIR} )

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

#
# WORKAROUND: replace DllImport("yui" with DllImport("/abs_path/yui.so"
#
ADD_CUSTOM_COMMAND(
	TARGET ${TARGETLIB}
	POST_BUILD
	COMMAND sed -i -e 's|DllImport\(\"yui\"|DllImport\(\"${MONO_LIBRARIES}/yui/yui.so\"|g' "${CMAKE_CURRENT_BINARY_DIR}/yuiPINVOKE.cs" )


ADD_CUSTOM_COMMAND(
   TARGET ${TARGETLIB} POST_BUILD
   COMMAND "${CSHARP_MONO_COMPILER_${CSHARP_MONO_VERSION}}" /t:library *.cs /out:yui.dll
   COMMENT "Creating yui.dll"
   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

INSTALL(TARGETS ${TARGETLIB} LIBRARY DESTINATION "${MONO_LIBRARIES}/yui")

INSTALL(
   FILES ${CMAKE_CURRENT_BINARY_DIR}/yui.dll
   DESTINATION "${MONO_LIBRARIES}/yui"
   COMPONENT csharp
)
