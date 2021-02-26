##### PROJECTINFO for CMake

SET( BASELIB		"yui" )		# don't change this

##### MAKE ALL NEEDED CHANGES HERE #####

SET( SUBDIRS		src examples )	# set the subdirs where your code is located.
SET( PLUGINNAME		qt-graph )	# define the your plugin's name, e.g. {gtk, ncurses-pkg, qt-graph}
SET( LIB_DEPS		Qt5Core Qt5Gui Qt5Widgets Qt5Svg Qt5X11Extras )	# define the plugin's dependecies CMake should look for separated with spaces; libyui-internal deps go in the next line
SET( INTERNAL_DEPS	Libyui Libyui-qt )	# include libyui or it's plugins (if needed), too!
#SET( LIB_LINKER		gvc graph cdt)		# define the libs to link against with their -l name for separated with spaces, e.g. pthread dl; include libyui or it's plugins (if needed), too!
SET( EXTRA_INCLUDES	/usr/include/graphviz )		# set include-dir which are not picked by CMake automagically here.
SET( PROGSUBDIR		)		# define your programm's name if the plugin is made for it, only. e.g. "YaST2" for {gtk,ncurses,qt}-pkg.


SET( URL                "http://github.com/libyui/" )
SET( SUMMARY            "Libyui - Qt Graph Widget" )
SET( DESCRIPTION        "This package contains the Qt graph \ncomponent for libYUI.\n" )


INCLUDE( "${CMAKE_SOURCE_DIR}/Libyui-qt-graphLibgvc.cmake" )
SET_LIBGVS_LIBS()


