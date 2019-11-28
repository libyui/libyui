# find the correct libraries depending on libgvc's version

MACRO( SET_LIBGVS_LIBS )
	EXECUTE_PROCESS(COMMAND /usr/bin/pkg-config --libs libgvc
	  RESULT_VARIABLE _exit
	  OUTPUT_VARIABLE _tmp
	  OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	IF(${_exit} GREATER 0)
	  MESSAGE(FATAL_ERROR "Install graphviz-devel")
	ENDIF()
	STRING(REPLACE "-l" "" _tmp "${_tmp}")
	SEPARATE_ARGUMENTS(_tmp)
	SET( LIB_LINKER ${_tmp} )
ENDMACRO( SET_LIBGVS_LIBS )


