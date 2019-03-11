##### PROJECTINFO for CMake

SET( BASELIB		"yui" )		# don't change this

##### MAKE ALL NEEDED CHANGES HERE #####

SET( PLUGINNAME         testframework )
SET( SUBDIRS		src )
SET( LIB_DEPS		Boost Qt5Core Qt5Gui Qt5Widgets Qt5Svg Qt5X11Extras) # define the plugin's dependecies CMake should look for separated with spaces; libyui-internal deps go in the next line
SET( LIB_LINKER		dl pthread microhttpd jsoncpp X11 fontconfig ) # define the libs to link against with their -l name for separated with spaces, e.g. pthread dl; include libyui or it's plugins (if needed), too!
SET( INTERNAL_DEPS Libyui Libyui-qt )
SET( EXTRA_INCLUDES     )         # set include-dir which are not picked by CMake automagically here.
SET( URL		"http://github.com/libyui/" )
SET( SUMMARY		"GUI-abstraction library" )
SET( DESCRIPTION	"This is the user interface engine that provides the abstraction from\n" )
SET( DESCRIPTION	"${DESCRIPTION}graphical user interfaces (Qt, Gtk) and text based user interfaces\n" )
SET( DESCRIPTION	"${DESCRIPTION}(ncurses).\n\n" )
SET( DESCRIPTION	"${DESCRIPTION}Originally developed for YaST, it can now be used independently of\n" )
SET( DESCRIPTION	"${DESCRIPTION}YaST for generic (C++) applications. This package has very few\n" )
SET( DESCRIPTION	"${DESCRIPTION}dependencies." )
