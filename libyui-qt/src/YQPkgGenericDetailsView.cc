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

  File:	      YQPkgGenericDetailsView.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

#define y2log_component "qt-pkg"
#include <ycp/y2log.h>
#include <qtabwidget.h>
#include <qregexp.h>

#include "YQPkgGenericDetailsView.h"
#include "YQi18n.h"
#include "utf8.h"


YQPkgGenericDetailsView::YQPkgGenericDetailsView( QWidget * parent )
    : QTextBrowser( parent )
{
    _pmObj = 0;
    _parentTab = dynamic_cast<QTabWidget *> (parent);
    
    if ( _parentTab )
    {
	connect( parent, SIGNAL( currentChanged( QWidget * ) ),
		 this,   SLOT  ( reload        ( QWidget * ) ) );
    }
}


YQPkgGenericDetailsView::~YQPkgGenericDetailsView()
{
    // NOP
}


void
YQPkgGenericDetailsView::reload( QWidget * newCurrent )
{
    if ( newCurrent == this )
    {
	showDetailsIfVisible( _pmObj );
    }
}


void
YQPkgGenericDetailsView::showDetailsIfVisible( PMObjectPtr pmObj )
{
    _pmObj = pmObj;
    
    if ( _parentTab )		// Is this view embedded into a tab widget?
    {
	if ( _parentTab->currentPage() == this )  // Is this page the topmost?
	{
	    showDetails( pmObj );
	}
    }
    else	// No tab parent - simply show data unconditionally.
    {
	showDetails( pmObj );
    }
}


QString
YQPkgGenericDetailsView::htmlHeading( PMObjectPtr pmObj )
{
    QString summary = fromUTF8( pmObj->summary() );
    
    QString html =
	"<table bgcolor=#E0E0F8><tr><td><b>"
	+ fromUTF8( pmObj->name() )
	+ "</b>";

    if ( ! summary.isEmpty() )
	html += " - " + summary;
    
    html += "</td></tr></table><br>";
    
    return html;
}


QString
YQPkgGenericDetailsView::htmlEscape( const QString & plainText )
{
    QString html = plainText;
    // y2debug( "Escaping '%s'", (const char *) plainText );

    html.replace( QRegExp( "&" ), "&amp;" );
    html.replace( QRegExp( "<" ), "&lt;"  );
    html.replace( QRegExp( ">" ), "&gt;"  );
    
    return html;
}


QSize
YQPkgGenericDetailsView::minimumSizeHint() const
{
    return QSize( 0, 0 );
}



#include "YQPkgGenericDetailsView.moc.cc"
