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
    _pkg = 0;
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
	showPkgDetailsIfVisible( _pkg );
    }
}


void
YQPkgGenericDetailsView::showPkgDetailsIfVisible( PMPackagePtr pkg )
{
    _pkg = pkg;
    
    if ( ! _parentTab )		// Is this view embedded into a tab widget?
    {
	if ( _parentTab->currentPage() == this )  // Is this page the topmost?
	{
	    showPkgDetails( pkg );
	}
    }
    else	// No tab parent - simply show data unconditionally.
    {
	showPkgDetails( pkg );
    }
}


QString
YQPkgGenericDetailsView::htmlHeading( PMPackagePtr pkg )
{
    QString summary = fromUTF8( pkg->summary() );
    
    QString html =
	"<table bgcolor=#E0E0F8><tr><td><b>"
	+ fromUTF8( pkg->name() )
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

    html.replace( QRegExp( "<" ), "&lt;"  );
    html.replace( QRegExp( ">" ), "&gt;"  );
    html.replace( QRegExp( "&" ), "&amp;" );
    
    return html;
}


#include "YQPkgGenericDetailsView.moc.cc"
