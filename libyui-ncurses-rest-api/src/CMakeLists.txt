# Find includes in corresponding build directories
set( CMAKE_INCLUDE_CURRENT_DIR ON )

PROCESS_SOURCES()
target_link_libraries(
  ${TARGETLIB}
  )
