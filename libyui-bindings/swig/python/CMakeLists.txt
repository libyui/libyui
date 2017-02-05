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

ADD_CUSTOM_COMMAND (
   OUTPUT  ${SWIG_OUTPUT}
   COMMAND ${CMAKE_COMMAND} -E echo_append "Creating wrapper code for Python ..."
   COMMAND ${SWIG_EXECUTABLE} -c++ -python -shadow -features autodoc ${SWIG_FLAGS} -o ${SWIG_OUTPUT} -I${LIBYUI_INCLUDE_DIR} ${SWIG_INPUT}
   COMMAND ${CMAKE_COMMAND} -E echo "Done."
   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../*.i
)

SET(yui_python_SRCS ${SWIG_OUTPUT} )

ADD_LIBRARY( yui_python SHARED ${yui_python_SRCS} )
# name it '_yui.so'
#  and don't prefix with 'lib'
SET_TARGET_PROPERTIES( yui_python PROPERTIES PREFIX "" OUTPUT_NAME "_yui")

INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_PATH} ${LIBYUI_INCLUDE_DIR} )

TARGET_LINK_LIBRARIES( yui_python ${LIBYUI_LIBRARY} )
TARGET_LINK_LIBRARIES( yui_python ${PYTHON_LIBRARIES} )

INSTALL(TARGETS yui_python LIBRARY DESTINATION ${PYTHON_SITEDIR})

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/yui.py DESTINATION ${PYTHON_SITEDIR} )
