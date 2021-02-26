##### PROJECTINFO for CMake

SET( BASELIB		"yui" )		# don't change this

##### MAKE ALL NEEDED CHANGES HERE #####

SET( SUBDIRS		src )
SET( PLUGINNAME		"ncurses-pkg" )
SET( LIB_DEPS		Curses6 Zypp )
SET( INTERNAL_DEPS	Libyui Libyui-ncurses )
SET( LIB_LINKER		)
SET( URL		"http://github.com/libyui/" )
SET( SUMMARY		"Libyui - yast2 package selector widget for the ncurses UI" )
SET( DESCRIPTION	"This package extends the character based (ncurses) user interface\ncomponent for libYUI.\n" )
