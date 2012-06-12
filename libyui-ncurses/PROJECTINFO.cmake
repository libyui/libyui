##### PROJECTINFO for CMake

SET( BASELIB		"yui" )		# don't change this

##### MAKE ALL NEEDED CHANGES HERE #####

SET( SUBDIRS		src )
SET( PLUGINNAME		"ncurses" )
SET( LIB_DEPS		Curses )
SET( INTERNAL_DEPS	Libyui )
SET( LIB_LINKER		ncurses ncursesw panelw )
