#
# Pkg icons
#

# Most of those icons are simply included in ../YQIconPool.cc,
# but some of them also need to be installed so they can be used
# in the "Symbols Help" dialog.

FILE( GLOB pkg_help_icons . *.xpm )

INSTALL( FILES       ${pkg_help_icons}
	 DESTINATION ${INSTALL_HELP_ICONS_DIR} )
