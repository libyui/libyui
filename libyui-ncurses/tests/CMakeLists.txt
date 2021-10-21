# To run the tests, use
#   make test
# or
#   make test ARGS=-VV
# see also
#   man ctest

# The tests are using the Boost.Test framework.
# Run a test binary with --help for all the options.
# To see the individual checks:
#
#    BOOST_TEST_COLOR_OUTPUT=1 BOOST_TEST_LOG_LEVEL=all make test ARGS=-V

set(BASELIB "yui-ncurses")

find_package(Boost COMPONENTS unit_test_framework REQUIRED)
include_directories(${Boost_INCLUDE_DIRS} ../src ${CMAKE_INCLUDE_PATH} )
add_definitions(-DBOOST_TEST_DYN_LINK -DTESTS_SRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}" )

set( LOCAL_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include )

# Symlink ../../libyui/src to build/src/include/yui
# so the headers there can be included as <yui/YFoo.h>
file( MAKE_DIRECTORY ${LOCAL_INCLUDE_DIR} )
file( CREATE_LINK ${CMAKE_CURRENT_SOURCE_DIR}/../../libyui/src ${LOCAL_INCLUDE_DIR}/yui SYMBOLIC )

# expect the tests in *_test.cc files
file(GLOB unit_tests "*_test.cc")
foreach(unit_test ${unit_tests})
  # strip the .cc suffix
  get_filename_component(unit_test_bin ${unit_test} NAME_WE)
  # build the test executable
  add_executable(${unit_test_bin} ${unit_test})
  add_dependencies(${unit_test_bin} "lib${BASELIB}")
  target_include_directories(${unit_test_bin} BEFORE PUBLIC ${LOCAL_INCLUDE_DIR} )
  target_link_directories(${unit_test_bin} BEFORE PUBLIC ../build/src)
  target_link_libraries (${unit_test_bin} ${BASELIB} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} )
  # add it to the test suite
  add_test(NAME ${unit_test_bin} COMMAND ${unit_test_bin})
endforeach(unit_test)

