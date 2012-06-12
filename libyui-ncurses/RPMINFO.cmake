##### RPMINFO for CMake
#####
##### THIS IS THE INFO REQUIRED FOR SPEC-FILE GENERATION

SET( SPEC_URL			"http://github.com/libyui/" )							# the URL of the project

SET( SPEC_SUMMARY		"" )										# some brief summary

SET( SPEC_DESCRIPTION		"\n" )										# the description to be used, end each line with "\n" for line-breaks

SET( SPEC_BuildRequires		"cmake >= 2.8" )								# the BuildRequires every single pkg wrapped with "" and speparated with spaces

SET( SPEC_Conflicts		"" )										# the Conflicts every single pkg wrapped with "" and speparated with spaces

SET( SPEC_Provides		"" )										# the Provides every single pkg wrapped with "" and speparated with spaces

SET( SPEC_Obsoletes		"" )										# the Obsoletes every single pkg wrapped with "" and speparated with spaces

SET( SPEC_DEVEL_Requires	"@PROJECTNAME@@@varpfx@_SONAME_MAJOR@ = %{version}" )				# the Requires for the -devel pkg every single pkg wrapped with "" and speparated with spaces

SET( SPEC_DEVEL_Provides	"pkgconfig(@PROJECTNAME@) = %{version}" )					# the Provides for the -devel pkg