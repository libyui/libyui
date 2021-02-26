##### PROJECTINFO for CMake

SET( BASELIB		"yui" )		# don't change this

##### MAKE ALL NEEDED CHANGES HERE #####

SET( PLUGINNAME         rest-api )
SET( SUBDIRS		src )
SET( LIB_DEPS		Boost ) # define the plugin's dependecies CMake should look for separated with spaces; libyui-internal deps go in the next line
SET( LIB_LINKER		dl pthread microhttpd jsoncpp ) # define the libs to link against with their -l name for separated with spaces, e.g. pthread dl; include libyui or it's plugins (if needed), too!
SET( EXTRA_INCLUDES     )         # set include-dir which are not picked by CMake automagically here.
SET( INTERNAL_DEPS  Libyui )
SET( URL		"http://github.com/libyui/" )
SET( SUMMARY		"GUI-abstraction library - the core REST API plugin" )
SET( DESCRIPTION	"This is the core of the REST API testing framework for the libyui library." )
