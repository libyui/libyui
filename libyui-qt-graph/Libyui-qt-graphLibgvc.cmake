# find the correct libraries depending on libgvc's version

MACRO( SET_LIBGVS_LIBS )
	EXEC_PROGRAM(/usr/bin/pkg-config ARGS \"libgvc\" \"--libs\" OUTPUT_VARIABLE _tmp)
	STRING(REPLACE " \n" "" _tmp "${_tmp}")
	STRING(REPLACE "-l" "" _tmp "${_tmp}")
	SEPARATE_ARGUMENTS(_tmp)
	SET( LIB_LINKER ${_tmp} )
ENDMACRO( SET_LIBGVS_LIBS )


