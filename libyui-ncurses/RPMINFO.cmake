##### RPMINFO for CMake
#####
##### THIS IS THE INFO REQUIRED FOR SPEC-FILE GENERATION

SET( SPEC_Url			"http://github.com/libyui/" )
SET( SPEC_Summary		"libYUI - Character Based User Interface" )
SET( SPEC_Description		"This package contains the character based (ncurses) user interface\ncomponent for libYUI.\n" )
SET( SPEC_BuildRequires		"cmake >= 2.8" "gcc-c++" "libtool" "libyui-devel" "ncurses-devel" )
SET( SPEC_Obsoletes		"yast2-ncurses < %{version}" )
SET( SPEC_DEVEL_Requires	"@PROJECTNAME@@@varpfx@_SONAME_MAJOR@ = %{version}" "libyui-devel" "ncurses-devel" )
SET( SPEC_DEVEL_Provides	"pkgconfig(@PROJECTNAME@) = %{version}" )
