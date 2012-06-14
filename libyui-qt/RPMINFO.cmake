##### RPMINFO for CMake
#####
##### THIS IS THE INFO REQUIRED FOR SPEC-FILE GENERATION

SET( SPEC_Url			"http://github.com/libyui/libyui-qt/" )						# the URL of the project

SET( SPEC_Summary		"Qt frontend for libyui" )							# some brief summary

SET( SPEC_Description		"This is the user interface frontend that provides Qt UI for libyui\n" )	# the description to be used, end each line with "\n" for line-breaks

SET( SPEC_BuildRequires		"cmake >= 2.8" "libyui-devel >= 2.21.5" "gcc-c++" "libqt4-devel" "fontconfig-devel" )			# the BuildRequires every single pkg wrapped with "" and speparated with spaces

SET( SPEC_Conflicts		"" )										# the Conflicts every single pkg wrapped with "" and speparated with spaces

SET( SPEC_Provides		"py2qt = %{version}" )                                                          # the Provides every single pkg wrapped with "" and speparated with spaces

SET( SPEC_Obsoletes		"py2qt < %{version}" )							        # the Obsoletes every single pkg wrapped with "" and speparated with spaces

SET( SPEC_Docs			"%doc COPYING.GPL2" )										# for docs that should be in the main-pkg

SET( SPEC_DEVEL_Requires	"@PROJECTNAME@@@varpfx@_SONAME_MAJOR@ = %{version}" )				# the Requires for the -devel pkg every single pkg wrapped with "" and speparated with spaces
SET( SPEC_DEVEL_Requires	${SPEC_DEVEL_Requires} "glibc-devel" "libstdc++-devel" )

SET( SPEC_DEVEL_Provides	"pkgconfig(@PROJECTNAME@) = %{version}" )					# the Provides for the -devel pkg

SET( SPEC_DEVEL_Docs		"" )										# for docs that should be in the devel-pkg
