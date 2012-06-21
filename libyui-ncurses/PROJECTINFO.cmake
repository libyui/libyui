##### PROJECTINFO for CMake

SET( BASELIB		"yui" )		# don't change this

##### MAKE ALL NEEDED CHANGES HERE #####

SET( SUBDIRS		src )
SET( PLUGINNAME		"ncurses" )
SET( LIB_DEPS		)
SET( INTERNAL_DEPS	Libyui )
SET( LIB_LINKER		)
SET( URL		"http://github.com/libyui/" )
SET( SUMMARY		"Libyui - Character Based User Interface" )
SET( DESCRIPTION	"This package contains the character based (ncurses) user interface\ncomponent for libYUI.\n" )

# using ncurses6:
SET ( NCURSES_CFLAGS	"-I/usr/include/ncurses6" )
SET ( NCURSES_LIBS	"-L/usr/lib64/ncurses6 -lncursesw -lpanelw" )

SET ( CMAKE_CXX_FLAGS	"${NCURSES_CFLAGS} ${CMAKE_CXX_FLAGS}" )
SET ( CMAKE_C_FLAGS	"${NCURSES_CFLAGS} ${CMAKE_C_FLAGS}" )
SET ( CMAKE_SHARED_LINKER_FLAGS	"${NCURSES_LIBS} ${CMAKE_SHARED_LINKER_FLAGS}" )
ADD_DEFINITIONS( "-DNCURSES_INTERNALS=1" )
