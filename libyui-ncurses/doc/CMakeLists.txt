# CMakeLists.txt for libyui-ncurses/doc

include( ../../VERSION.cmake )

find_package( Doxygen REQUIRED dot )


set( PKG_NAME           libyui-ncurses${SONAME_MAJOR} )
set( DOC_INSTALL_DIR    ${CMAKE_INSTALL_DATAROOTDIR}/doc/packages/${PKG_NAME} )


#
# Doxygen-generated autodocs
#

if ( DOXYGEN_FOUND )

  set( DOXYGEN_GENERATE_TREEVIEW yes ) # Enable views tree HTML frame
  set( DOXYGEN_QUIET             yes ) # Less verbose output
  set( DOXYGEN_WARN_LOGFILE      doxygen-warnings.log )
  # See build/CMakeDoxyfile.in for more supported variables

  doxygen_add_docs( doc ../src ALL )
  install( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html
    DESTINATION ${DOC_INSTALL_DIR} )

else()
  message( WARNING "Missing doxygen package" )
endif()


#
# Other (manually written) docs
#

file( GLOB DOC_FILES *.md ) # Collect all *.md files by wildcard
# Don't glob *.txt (CMakeLists.txt!)
# The license files (../COPYING*) are handled in the .spec file

install( FILES ${DOC_FILES} DESTINATION ${DOC_INSTALL_DIR} )

# used the standard "make install" way from:
# https://gist.github.com/shundhammer/12b6343309544a5920f51cd0063eeecf

