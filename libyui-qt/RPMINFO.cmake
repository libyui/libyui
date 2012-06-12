##### RPMINFO for CMake
#####
##### THIS IS THE INFO REQUIRED FOR SPEC-FILE GENERATION

SET( SPEC_URL			"http://github.com/libyui/libyui-qt/" )						# the URL of the project

SET( SPEC_SUMMARY		"Qt frontend for libyui" )							# some brief summary

SET( SPEC_DESCRIPTION		"This is the user interface frontend that provides Qt UI for libyui\n" )	# the description to be used, end each line with "\n" for line-breaks

SET( SPEC_BuildRequires		"libyui-devel >= 2.21.5"  "libqt4-devel" "cmake >= 2.8" "gcc-c++" )			# the BuildRequires every single pkg wrapped with "" and speparated with spaces

SET( SPEC_Provides		"py2qt = %{version}" )                                                          # the Provides every single pkg wrapped with "" and speparated with spaces

SET( SPEC_Obsoletes		"py2qt < %{version}" )							        # the Obsoletes every single pkg wrapped with "" and speparated with spaces

SET( SPEC_DEVEL_Requires	"@PROJECTNAME@@@varpfx@_SONAME_MAJOR@ = %{version}" )				# the Requires for the -devel pkg every single pkg wrapped with "" and speparated with spaces
SET( SPEC_DEVEL_Requires	${SPEC_DEVEL_Requires} "glibc-devel" "libstdc++-devel" )

SET( SPEC_DEVEL_Provides	"pkgconfig(@PROJECTNAME@) = %{version}" )					# the Provides for the -devel pkg
