##### RPMINFO for CMake
#####
##### THIS IS THE INFO REQUIRED FOR SPEC-FILE GENERATION

SET( SPEC_Url			"http://github.com/libyui/" )
SET( SPEC_Summary		"Libyui - Character Based User Interface" )
SET( SPEC_Description		"This package contains the character based (ncurses) user interface\ncomponent for libYUI.\n" )
SET( SPEC_BuildRequires		"cmake >= 2.8" "gcc-c++" "libtool" "libyui-devel" "ncurses-devel" )
SET( SPEC_Provides		"yast2-ncurses = 2.22.4" )
SET( SPEC_Obsoletes		"yast2-ncurses < 2.22.4" )
SET( SPEC_Docs                  "%doc %dir %{_docdir}/@PROJECTNAME@@SONAME_MAJOR@\n%doc %{_docdir}/@PROJECTNAME@@SONAME_MAJOR@/COPYING*" )
SET( SPEC_DEVEL_Requires	"@PROJECTNAME@@@varpfx@SONAME_MAJOR@ = %{version}" "libyui-devel" "ncurses-devel" )
SET( SPEC_DEVEL_Provides	"pkgconfig(@PROJECTNAME@) = %{version}" )
