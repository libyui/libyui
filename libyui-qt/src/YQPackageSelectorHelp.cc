/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

  File:	      YQPackageSelectorHelp.cc
  See also:   YQPackageSelector.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  Textdomain "packages-qt"

/-*/

#include <qstring.h>

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include "YQPackageSelector.h"
#include "YQPkgTextDialog.h"

#include "utf8.h"
#include "YQi18n.h"



void
YQPackageSelector::help()
{
    QString text = _( "<p>Help text needs to be written!</p>" );
#warning TODO: Generic help text
    
    YQPkgTextDialog::showText( this, text );
}


void
YQPackageSelector::symbolHelp()
{
    // Translators: Headline for help about package status icons
    QString html = YQPkgTextDialog::htmlHeading( _( "Symbols Overview" ) );

    html += "<table border=1>";
    
    html += symHelp( "noinst.xpm",
		     // Translators: Package status short (!) description
		     _( "Don't install" ),
		     // Translators: Automatic word-wrapping.
		     _( "This package is not installed, and it will not be installed." ) );
    
    html += symHelp( "install.xpm",
		     // Translators: Package status short (!) description
		     _( "Install" ),
		     // Translators: Automatic word-wrapping.
		     _( "This package will be installed. It is not installed yet." ) );
    
    html += symHelp( "keepinstalled.xpm",
		     // Translators: Package status short (!) description
		     _( "Keep" ),
		     // Translators: Automatic word-wrapping.
		     _( "This package is already installed. Keep it untouched." ) );

    html += symHelp( "update.xpm",
		     // Translators: Package status short (!) description
		     _( "Update" ),
		     // Translators: Automatic word-wrapping.
		     _( "This package is already installed. Update it or reinstall it"
			" (if the versions are the same)." ) );

    html += symHelp( "del.xpm",
		     // Translators: Package status short (!) description
		     _( "Delete" ),
		     // Translators: Automatic word-wrapping.
		     _( "This package is already installed. Delete it." ) );

    html += symHelp( "taboo.xpm",
		     // Translators: Package status short (!) description
		     _( "Taboo" ),
		     // Translators: Automatic word-wrapping.
		     _( "This package is not installed, and I don't want it under any circumstances -"
			" in particular not because of unresolved dependencies that other packages"
			" might have or get." )
		     + " "
		     + _( "Packages set to \"taboo\" are treated as if they didn't exist on any installation media." ) );
    

    html += symHelp( "autoinstall.xpm",
		     // Translators: Package status short (!) description
		     _( "Auto-install" ),
		     // Translators: Automatic word-wrapping.
		     _( "This package will be installed automatically because some other package needs it"
			" or because it is contained in a predefined software selection"
			" (e.g., \"Multimedia\", \"Development\" )." )
		     + " "
		     + _( "<b>Hint:</b> You may have to use \"taboo\" to get rid of such a package." ) );
    
    html += symHelp( "autoupdate.xpm",
		     // Translators: Package status short (!) description
		     _( "Auto-update" ),
		     // Translators: Automatic word-wrapping.
		     _( "This package is already installed, but some other package (or selection)"
			" needs a newer version, so it will automatically be updated." ) );
		     
    html += symHelp( "autodel.xpm",
		     // Translators: Package status short (!) description
		     _( "Auto-delete" ),
		     // Translators: Automatic word-wrapping.
		     _( "This package is already installed, but some predefined software selection"
			"(e.g., \"Multimedia\", \"Development\" ) requires that it is deleted." ) );

    html += "/<table>";

    
    YQPkgTextDialog::showText( this, html );
}


QString
YQPackageSelector::symHelp( const QString & imgFileName,
			    const QString & summary,
			    const QString & explanation		)
{
    QString imgPath = HELPIMAGEDIR;
    QString html = "<tr valign=top>";
    html += "<td><img src=\"" + imgPath + "/" + imgFileName + "\"></td>"
	+ "<td>" + summary 	+ "</td>"
	+ "<td>" + explanation 	+ "</td>"
	+ "</tr>";

    return html;
}
    

void
YQPackageSelector::keyboardHelp()
{
    // Translators: Headline for help about "magic keys" in the package manager
    QString html = YQPkgTextDialog::htmlHeading( _( "Special Keys Overview" ) );

    // Translators: Additional hint that will be added to any of the following keyboard actions.
    // Automatic word wrapping.
    QString goto_next = "<br>" + _( "In any case, go to the next list item." );

    html += "<table border=1>";
    
    html += keyHelp( "+",
		     // Translators: Keyboard action short (!) description
		     _( "Add" ),
		     // Translators: Automatic word-wrapping.
		     _( "Get this package. Install it if it isn't installed yet,"
			" update it to the latest version if it is installed "
			" and there is a newer version." )
		     + goto_next );

    html += keyHelp( "-",
		     // Translators: Keyboard action short (!) description
		     _( "Remove" ),
		     // Translators: Automatic word-wrapping.
		     _( "Get rid of this package. Mark it as \"don't install\" "
			"if it isn't installed yet, delete it if it is installed." )
		     + goto_next );
    
    html += keyHelp( "&gt;",
		     // Translators: Keyboard action short (!) description
		     _( "Update" ),
		     // Translators: Automatic word-wrapping.
		     _( "Update this package if it is installed and there is a newer version."
			" Ignore packages that are not installed." )
		     + goto_next );

    
    html += keyHelp( "&lt;",
		     // Translators: Keyboard action short (!) description
		     _( "Undo update" ),
		     // Translators: Automatic word-wrapping.
		     _( "Undo the effect of \">\" above: Set package to \"keep\""
			" if it is currently set to \"update\". Ignore all other packages." )
		     + goto_next );

    html += keyHelp( "!",
		     // Translators: Keyboard action short (!) description
		     _( "Taboo" ),
		     // Translators: Automatic word-wrapping.
		     _( "Set this package to \"taboo\" if it isn't installed:"
			" Make sure this package doesn't get installed, in particular not"
			" because of unresolved dependencies that other packages might have or get. " )
		     + " "
		     + _( "Packages set to \"taboo\" are treated as if they didn't exist on any installation media." )
		     + "<br>"
		     + goto_next );
    
    html += "/<table>";

    
    YQPkgTextDialog::showText( this, html );
}


QString
YQPackageSelector::keyHelp( const QString & key,
			    const QString & summary,
			    const QString & explanation		)
{
    QString html = "<tr valign=top>";
    html +=
	"<td><table bgcolor=#E0E0E0><tr><td><b>"
	+ key
	+ "</b></td></tr></table></td>"
	+ "<td>" + summary 	+ "</td>"
	+ "<td>" + explanation 	+ "</td>"
	+ "</tr>";

    return html;
}
    
