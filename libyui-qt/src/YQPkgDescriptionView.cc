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

#include "YQPkgDescriptionView.h"
#include "YQi18n.h"
#include "utf8.h"


YQPkgDescriptionView::YQPkgDescriptionView( QWidget * parent )
    : YQPkgGenericDetailsView( parent )
{
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

    bool auto_format = true;
    std::list<std::string> description = pmObj->description();
    std::list<std::string>::const_iterator it = description.begin();

    while ( it != description.end() )
    {
	QString line = fromUTF8( *it );
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
	
	++it;
    }

    html_text += "</p>";

    setTextFormat( Qt::RichText );
    setText( html_text );
}



#include "YQPkgDescriptionView.moc.cc"
