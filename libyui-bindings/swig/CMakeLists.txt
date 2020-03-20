#
# Generic setting for all generated bindings
#
SET( SWIG_INPUT "${CMAKE_CURRENT_SOURCE_DIR}/yui.i" )

OPTION (WITH_MONO "Build CSharp/Mono bindings" ON)
OPTION (WITH_PYTHON "Build Python bindings" ON)
OPTION (WITH_PERL "Build Perl bindings" ON)
OPTION (WITH_RUBY "Build Ruby bindings" ON)


IF (WITH_RUBY)
  FIND_PACKAGE(Ruby)
  IF (RUBY_LIBRARY AND RUBY_INCLUDE_PATH)
    ADD_SUBDIRECTORY(ruby)
  ENDIF(RUBY_LIBRARY AND RUBY_INCLUDE_PATH)
ENDIF (WITH_RUBY)

IF (WITH_PYTHON)
  FIND_PACKAGE(PythonLibs)
  IF (PYTHON_LIBRARY)
    ADD_SUBDIRECTORY(python)
  ENDIF(PYTHON_LIBRARY)
ENDIF (WITH_PYTHON)

IF (WITH_PERL)
  # doesn't work yet :-(
  # CMake problem, FindPerlLibs.cmake is in a _really_ bad, bad state :-((
  # see e.g. http://www.vtk.org/Bug/print_bug_page.php?bug_id=3665
  FIND_PACKAGE(PerlLibs)
  #FindPerl.cmake included by FindPerlLibs.cmake
  #FIND_PACKAGE(Perl)
  IF (PERL_EXECUTABLE)
    MESSAGE(STATUS "Perl executable: ${PERL_EXECUTABLE}")
    ADD_SUBDIRECTORY(perl)
  ENDIF (PERL_EXECUTABLE)
ENDIF (WITH_PERL)

IF (WITH_MONO)
  FIND_PACKAGE(Mono)
  IF (CSHARP_MONO_FOUND)
    ADD_SUBDIRECTORY(mono)
  ENDIF (CSHARP_MONO_FOUND)
ENDIF (WITH_MONO)
