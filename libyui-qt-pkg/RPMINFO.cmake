##### RPMINFO for CMake
#####
##### THIS IS THE INFO REQUIRED FOR SPEC-FILE GENERATION

SET( SPEC_Url			"http://github.com/libyui-qt-pkg/" )							# the URL of the project

SET( SPEC_Summary		"Libyui - yast2 package selector widget for the Qt UI" )	# some brief summary

SET( SPEC_Description
  "This is the yast2 package selector widget plugin provided for the libyui Qt UI\n" )		# the description to be used, end each line with "\n" for line-breaks

SET( SPEC_BuildRequires		"cmake >= 2.8" "gcc-c++" "doxygen" "libqt4-devel"
  "libyui-devel"
  "libyui-qt-devel >= 2.21.6"
  "libzypp-devel >= 11.4.0"
)								# the BuildRequires every single pkg wrapped with "" and speparated with spaces

SET( SPEC_Requires		"" )										# the Requires every single pkg wrapped with "" and speparated with spaces

SET( SPEC_Provides		"yast2-qt-pkg = 2.21.25" )	# the Provides every single pkg wrapped with "" and speparated with spaces

SET( SPEC_Obsoletes		"yast2-qt-pkg < 2.21.25" )	# the Obsoletes every single pkg wrapped with "" and speparated with spaces

SET( SPEC_Conflicts		"" )										# the Conflicts every single pkg wrapped with "" and speparated with spaces

SET( SPEC_Docs			"" )				# for docs that should be in the main-pkg

SET( SPEC_DEVEL_Requires	"@PROJECTNAME@@@varpfx@SONAME_MAJOR@ = %{version}"
  "libyui-qt-devel >= 2.21.6"
)	# the Requires for the -devel pkg every single pkg wrapped with "" and speparated with spaces

SET( SPEC_DEVEL_Provides	"pkgconfig(@PROJECTNAME@) = %{version}"
  "yast2_ui_pkg"
)		# the Provides for the -devel pkg

SET( SPEC_DEVEL_Docs		"" )							# for docs that should be in the devel-pkg
