##### PROJECTINFO for CMake

SET( BASELIB		"yui" )		# don't change this

##### MAKE ALL NEEDED CHANGES HERE #####

SET( SUBDIRS		src examples )	# set the subdirs where your code is located.
SET( PLUGINNAME		qt-pkg )	# define the your plugin's name, e.g. {gtk, ncurses-pkg, qt-graph}
SET( LIB_DEPS		Qt5Core Qt5Gui Qt5Widgets Zypp )	# define the plugin's dependecies CMake should look for separated with spaces; libyui-internal deps go in the next line
SET( INTERNAL_DEPS	Libyui Libyui-qt )	# include libyui or it's plugins (if needed), too!
SET( LIB_LINKER		zypp )		# define the libs to link against with their -l name for separated with spaces, e.g. pthread dl; include libyui or it's plugins (if needed), too!
SET( EXTRA_INCLUDES	)		# set include-dir which are not picked by CMake automagically here.
SET( PROGSUBDIR		)		# define your programm's name if the plugin is made for it, only. e.g. "YaST2" for {gtk,ncurses,qt}-pkg.
SET( URL                "http://github.com/libyui/" )
SET( SUMMARY            "Libyui - Qt Package Selector" )
SET( DESCRIPTION        "This package contains the Qt package selector\ncomponent for libYUI.\n" )



## Things to cleanup; probably too yast2 related

IF( NOT PREFIX )
  SET( PREFIX "/usr" )
ENDIF( NOT PREFIX )

# used in src/icons and YQPackageSelectorHelp.cc (via -DHELPIMAGEDIR) probably specific to this file
SET( INSTALL_HELP_ICONS_DIR 	"${PREFIX}/share/YaST2/images" )

# maybe '#include <yui/Libyui_config.h>' rather than using '-DTHEMEDIR'
SET( YUIQT_THEMEDIR		"${PREFIX}/share/YaST2/theme" )
