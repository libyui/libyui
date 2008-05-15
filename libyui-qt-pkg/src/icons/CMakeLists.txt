#
# Pkg icons
#

# Most of those icons are simply included in ../YQIconPool.cc,
# but some of them also need to be installed so they can be used
# in the "Symbols Help" dialog.

SET( pkg_help_icons

     autodel.xpm
     autoinstall.xpm
     autoupdate.xpm
     del.xpm
     install.xpm
     keepinstalled.xpm
     noinst.xpm
     protected.xpm
     sel_autodel.xpm
     sel_autoinstall.xpm
     sel_autoupdate.xpm
     taboo.xpm
     update.xpm
)

INSTALL( FILES       ${pkg_help_icons}
	 DESTINATION ${CMAKE_INSTALL_PREFIX}/share/YaST2/images )
