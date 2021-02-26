##### PROJECTINFO for CMake

SET( BASELIB		"yui" )		# don't change this

##### MAKE ALL NEEDED CHANGES HERE #####

SET( PLUGINNAME         ncurses-rest-api )
SET( SUBDIRS		src )
SET( LIB_DEPS		Boost ) # define the plugin's dependecies CMake should look for separated with spaces; libyui-internal deps go in the next line
SET( LIB_LINKER		dl pthread ) # define the libs to link against with their -l name for separated with spaces, e.g. pthread dl; include libyui or it's plugins (if needed), too!
SET( EXTRA_INCLUDES     )         # set include-dir which are not picked by CMake automagically here.
SET( INTERNAL_DEPS  Libyui Libyui-ncurses Libyui-rest-api )
SET( URL		"http://github.com/libyui/" )
SET( SUMMARY		"GUI-abstraction library - the REST API bindings for the ncurses UI plugin" )
SET( DESCRIPTION	"This is the REST API plugin for the ncurses UI." )
