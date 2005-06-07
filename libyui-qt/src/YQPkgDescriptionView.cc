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

  File:	      YQPkgDescriptionView.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include "qregexp.h"
#include "YQPkgDescriptionView.h"
#include "YQPkgDescriptionDialog.h"
#include "YQi18n.h"
#include "utf8.h"

using std::list;
using std::string;


YQPkgDescriptionView::YQPkgDescriptionView( QWidget * parent )
    : YQPkgGenericDetailsView( parent )
{
    setMimeSourceFactory( 0 );
}


YQPkgDescriptionView::~YQPkgDescriptionView()
{
    // NOP
}


void
YQPkgDescriptionView::showDetails( PMObjectPtr pmObj )
{
    _pmObj = pmObj;

    if ( ! pmObj )
    {
	clear();
	return;
    }

    QString html_text = htmlHeading( pmObj );

    string name = pmObj->name();
    // y2debug( "Showing description for package %s", name.c_str() );


    // Add all lines of the package description

    bool auto_format  = true;
    bool preformatted = false;
    list<string> description = pmObj->description();
    list<string>::const_iterator it = description.begin();

    if ( it != description.end()
	 && *it == "<!-- DT:Rich -->" )	// Special doctype for preformatted HTML
    {
	preformatted = true;
	++it;					// Discard doctype line
    }


    while ( it != description.end() )
    {
	QString line = fromUTF8( *it );

	if ( preformatted )
	{
	    html_text += line + "\n";
	}
	else
	{
	    line = htmlEscape( line );

	    if ( line.startsWith( "Authors:" ) )
	    {
		line = "<b>" + line + "</b>";
		auto_format = false;
	    }

	    if ( auto_format )
	    {
		if ( line.length() == 0 )	// Empty lines mean new paragraph
		    html_text += "</p><p>";
		else
		    html_text += " " + line;
	    }
	    else
	    {
		html_text += line + "<br>";
	    }
	}

	++it;
    }

    if ( ! preformatted )
	html_text += "</p>";

    setTextFormat( Qt::RichText );
    setText( html_text );
    ensureVisible( 0, 0 );	// Otherwise hyperlinks will be centered
}


void
YQPkgDescriptionView::showLink( const QString & url )
{
    if ( url.startsWith( "pkg:" ) )
    {
	QString pkgName = url;
	pkgName.remove( QRegExp( "^pkg:/*" ) );	// Remove leading protocol and slashes
	pkgName.remove( QRegExp( "/*$" ) );	// Remove trailing slashes
	y2milestone( "Hyperlinking to package '%s'", (const char *) pkgName );
	YQPkgDescriptionDialog::showDescriptionDialog( pkgName );
    }
    else
    {
	y2error( "Protocol not supported - can't follow hyperlink '%s'",
		 (const char *) url );
    }
}


void
YQPkgDescriptionView::setSource( const QString & url )
{
    showLink( url );
}


#include "YQPkgDescriptionView.moc"
