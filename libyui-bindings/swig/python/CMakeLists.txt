#
# cmake description for libyui Python bindings
#
# !!Attn!!: This creates two files
#  1. yui.so
#  2. yui.py
# and the .py file gets imported. See bindings/python/tests.
#

# SWIG_OUPUT is per-target
SET( SWIG_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/yui_python.cxx" )


FIND_PACKAGE(PythonInterp)
EXECUTE_PROCESS(COMMAND ${PYTHON_EXECUTABLE} -c "import sys; sys.stdout.write(sys.path[2])" OUTPUT_VARIABLE PYTHON_LIB_DIR)

IF (NOT PYTHON_SITEDIR)
  SET (PYTHON_SITEDIR ${PYTHON_LIB_DIR}/site-packages)
  STRING(REGEX REPLACE "^/usr/" "${CMAKE_INSTALL_PREFIX}/" PYTHON_SITEDIR "${PYTHON_SITEDIR}")
ENDIF (NOT PYTHON_SITEDIR)

MESSAGE(STATUS "Python executable: ${PYTHON_EXECUTABLE}")
MESSAGE(STATUS "Python include dir: ${PYTHON_INCLUDE_PATH}")
MESSAGE(STATUS "Python lib dir: ${PYTHON_LIB_DIR}")
MESSAGE(STATUS "Python site dir: ${PYTHON_SITEDIR}")

set( LOCAL_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include )

# Symlink ../../../libyui/src to build/src/include/yui
# so the headers there can be included as <yui/YFoo.h>
file( MAKE_DIRECTORY ${LOCAL_INCLUDE_DIR} )
file( CREATE_LINK ${CMAKE_CURRENT_SOURCE_DIR}/../../../libyui/src ${LOCAL_INCLUDE_DIR}/yui SYMBOLIC )

ADD_CUSTOM_COMMAND (
   OUTPUT  ${SWIG_OUTPUT}
   COMMAND ${CMAKE_COMMAND} -E echo_append "Creating wrapper code for Python ..."
   COMMAND ${SWIG_EXECUTABLE} -c++ -python -shadow -features autodoc ${SWIG_FLAGS} -o ${SWIG_OUTPUT} -I${LOCAL_INCLUDE_DIR} -I${LIBYUI_INCLUDE_DIR} ${SWIG_INPUT}
   COMMAND ${CMAKE_COMMAND} -E echo "Done."
   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../*.i
)

SET(yui_python_SRCS ${SWIG_OUTPUT} )

set( TARGETLIB yui_python )
ADD_LIBRARY( ${TARGETLIB} SHARED ${yui_python_SRCS} )
# name it '_yui.so'
#  and don't prefix with 'lib'
SET_TARGET_PROPERTIES( ${TARGETLIB} PROPERTIES PREFIX "" OUTPUT_NAME "_yui")

target_include_directories( ${TARGETLIB} BEFORE PUBLIC ${LOCAL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_PATH} ${LIBYUI_INCLUDE_DIR} )

# Find yui during a combined build
target_link_directories( ${TARGETLIB} BEFORE PUBLIC ../../../libyui/build/src )

TARGET_LINK_LIBRARIES( ${TARGETLIB} ${LIBYUI_LIBRARY} )
TARGET_LINK_LIBRARIES( ${TARGETLIB} ${PYTHON_LIBRARIES} )

INSTALL(TARGETS ${TARGETLIB} LIBRARY DESTINATION ${PYTHON_SITEDIR})

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/yui.py DESTINATION ${PYTHON_SITEDIR} )
