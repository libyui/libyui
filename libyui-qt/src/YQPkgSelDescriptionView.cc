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

  File:	      YQPkgSelDescriptionView.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include "qregexp.h"

#include <Y2PM.h>
#include <y2pm/PMSelection.h>

#include "YQPkgSelDescriptionView.h"
#include "YQUI.h"
#include "YQi18n.h"
#include "utf8.h"

using std::list;
using std::string;


YQPkgSelDescriptionView::YQPkgSelDescriptionView( QWidget * parent )
    : YQPkgDescriptionView( parent )
{
}


YQPkgSelDescriptionView::~YQPkgSelDescriptionView()
{
    // NOP
}


void
YQPkgSelDescriptionView::showDetails( PMObjectPtr pmObj )
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



QString
YQPkgSelDescriptionView::htmlHeading( PMObjectPtr pmObj )
{
    PMSelectionPtr sel = pmObj;

    if ( ! sel )
	return YQPkgGenericDetailsView::htmlHeading( pmObj );
    
    QString summary = fromUTF8( sel->summary( Y2PM::getPreferredLocale() ) );
    bool useBigFont = ( summary.length() <= 40 );

    if ( summary.isEmpty() )
	summary = fromUTF8( pmObj->name() );

    QString html = "<table";

    if ( ! YQUI::ui()->usingVisionImpairedPalette() )
	html += " bgcolor=#C8C8F8";	// or #E0E0F8 (very light blueish grey)

    html += "><tr><td>"
	+ ( useBigFont ? QString( "<h2>" ) : QString( "<b>" ) )
	+ summary
	+ ( useBigFont ? QString( "</h2>" ) : QString( "</b>" ) )
	+ "</td></tr>"
	+ "</table><br>";

    return html;
}




#include "YQPkgSelDescriptionView.moc"
