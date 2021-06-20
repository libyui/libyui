#
# CMakeLists.txt for libyui/swig/ruby
#

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing")

EXECUTE_PROCESS(COMMAND ${RUBY_EXECUTABLE} -r rbconfig -e "print RbConfig::CONFIG['vendorarchdir']" OUTPUT_VARIABLE RUBY_VENDOR_ARCH_DIR)
STRING(REGEX REPLACE "^/usr/" "${CMAKE_INSTALL_PREFIX}/" RUBY_VENDOR_ARCH_DIR "${RUBY_VENDOR_ARCH_DIR}")

# 3.18.0: "Rename variables to match case of module name"
if(
   (("${CMAKE_MAJOR_VERSION}" GREATER_EQUAL 3) AND ("${CMAKE_MINOR_VERSION}" GREATER_EQUAL 18)) OR
   ("${CMAKE_MAJOR_VERSION}" GREATER_EQUAL 4)
  )
  SET(RUBY_VENDORLIB_DIR "${Ruby_VENDORLIB_DIR}")
endif()

MESSAGE(STATUS "Ruby executable: ${RUBY_EXECUTABLE}")
MESSAGE(STATUS "Ruby vendor arch dir: ${RUBY_VENDOR_ARCH_DIR}")
MESSAGE(STATUS "Ruby vendor lib dir: ${RUBY_VENDORLIB_DIR}")
MESSAGE(STATUS "Ruby include path: ${RUBY_INCLUDE_PATH}")

SET( SWIG_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/yui_ruby.cxx" )

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/yui/version.rb.in ${CMAKE_CURRENT_BINARY_DIR}/yui/version.rb)


set( LOCAL_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include )

# Symlink ../../../libyui/src to build/src/include/yui
# so the headers there can be included as <yui/YFoo.h>
file( MAKE_DIRECTORY ${LOCAL_INCLUDE_DIR} )
file( CREATE_LINK ${CMAKE_CURRENT_SOURCE_DIR}/../../../libyui/src ${LOCAL_INCLUDE_DIR}/yui SYMBOLIC )
if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../../../libyui-mga/src)
  file( CREATE_LINK ${CMAKE_CURRENT_SOURCE_DIR}/../../../libyui-mga/src ${LOCAL_INCLUDE_DIR}/yui/mga SYMBOLIC )
endif()

ADD_CUSTOM_COMMAND (
   OUTPUT  ${SWIG_OUTPUT}
   COMMAND ${CMAKE_COMMAND} -E echo_append "Creating wrapper code for ruby..."
   COMMAND ${SWIG_EXECUTABLE} -c++ -ruby -autorename ${SWIG_FLAGS} -o ${SWIG_OUTPUT} -I${LOCAL_INCLUDE_DIR} ${LIBYUI_INCLUDE_CFLAGS} ${SWIG_INPUT}
   COMMAND ${CMAKE_COMMAND} -E echo "Done."
   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../*.i
)

set( TARGETLIB yui_ruby )
ADD_LIBRARY( ${TARGETLIB} SHARED ${SWIG_OUTPUT} )

target_include_directories( ${TARGETLIB} BEFORE PUBLIC ${LOCAL_INCLUDE_DIR} )

INCLUDE_DIRECTORIES( ${RUBY_INCLUDE_PATH} ${LIBYUI_INCLUDE_DIR} )

SET_TARGET_PROPERTIES( ${TARGETLIB} PROPERTIES PREFIX "" OUTPUT_NAME "_yui")

if ( WITH_MGA)
  if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../../../libyui-mga/build/src)
    # Find yui and yui-mga during a combined build
    target_link_directories( ${TARGETLIB}
      BEFORE PUBLIC ../../../libyui/build/src
      BEFORE PUBLIC ../../../libyui-mga/build/src
    )
  else()
    # Using yui and yui-mga from system
    target_link_directories( ${TARGETLIB}
      PUBLIC ${YUI_LIBRARY_DIRS}
      PUBLIC ${YUIMGA_LIBRARY_DIRS}
    )
  endif()
else()
  # Find yui during a combined build
  target_link_directories( ${TARGETLIB} BEFORE PUBLIC ../../../libyui/build/src )
endif()

TARGET_LINK_LIBRARIES( ${TARGETLIB} ${LIBYUI_LIBRARY} )
TARGET_LINK_LIBRARIES( ${TARGETLIB} ${RUBY_LIBRARY} )

INSTALL(TARGETS ${TARGETLIB} LIBRARY DESTINATION ${RUBY_VENDOR_ARCH_DIR})

IF( BUILD_RUBY_GEM )
#
# Ruby GEM
#

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/yui.gemspec.in ${CMAKE_CURRENT_BINARY_DIR}/yui.gemspec)

SET(GEM_NAME "yui-${VERSION}.gem")
ADD_CUSTOM_TARGET(ruby_gem ALL DEPENDS ${GEM_NAME})
ADD_DEPENDENCIES(ruby_gem ${SWIG_OUTPUT})

ADD_CUSTOM_COMMAND (
   OUTPUT  ${GEM_NAME}
   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
   COMMAND "gem" "build" "${CMAKE_CURRENT_BINARY_DIR}/yui.gemspec"
   DEPENDS "ext"
   DEPENDS ${rdoc_dir}
   DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/yui.gemspec
)

#
# Prep GEM files
# gemspec needs _relative_ pathes
# so create ext and lib here and copy files
#
ADD_CUSTOM_COMMAND (
  OUTPUT "ext"
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMAND "rm" "-rf" "lib"
  COMMAND "rm" "-rf" "ext"
  COMMAND "mkdir" "-p" "ext/yui"
  COMMAND "cp" "${CMAKE_CURRENT_BINARY_DIR}/yui_ruby.cxx" "ext/yui"
  COMMAND "cp" "${CMAKE_CURRENT_SOURCE_DIR}/extconf.rb" "ext/yui"

  COMMAND "mkdir" "-p" "lib/yui"
  COMMAND "cp" "${CMAKE_CURRENT_SOURCE_DIR}/yui.rb" "lib"
  COMMAND "cp" "${CMAKE_CURRENT_SOURCE_DIR}/yui/*.rb" "lib/yui"
  COMMAND "cp" "${CMAKE_CURRENT_BINARY_DIR}/yui/*.rb" "lib/yui"

  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/extconf.rb
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/yui.rb
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/yui/*.rb
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/yui/*.rb.in
  DEPENDS ${SWIG_OUTPUT}
)
ELSE( BUILD_RUBY_GEM )
# install ruby files to vendor dir
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/yui.rb DESTINATION ${RUBY_VENDORLIB_DIR} )
FILE(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/yui/*.rb")
INSTALL(FILES ${files} DESTINATION ${RUBY_VENDORLIB_DIR}/yui)
FILE(GLOB files_built "${CMAKE_CURRENT_BINARY_DIR}/yui/*.rb")
INSTALL(FILES ${files_built} DESTINATION ${RUBY_VENDORLIB_DIR}/yui)
ENDIF( BUILD_RUBY_GEM )
