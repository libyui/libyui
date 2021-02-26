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
#include <qdatetime.h>

#include "YQPkgGenericDetailsView.h"
#include "YQi18n.h"
#include "YQUI.h"
#include "utf8.h"

using std::string;


YQPkgGenericDetailsView::YQPkgGenericDetailsView( QWidget * parent )
    : QTextBrowser( parent )
{
    _selectable = 0;
    _parentTab  = dynamic_cast<QTabWidget *> (parent);

    if ( _parentTab )
    {
	connect( parent, SIGNAL( currentChanged(QWidget *) ),
		 this,   SLOT  ( reload        (QWidget *) ) );
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
	showDetailsIfVisible( _selectable );
    }
}


void
YQPkgGenericDetailsView::showDetailsIfVisible( ZyppSel selectable )
{
    _selectable = selectable;

    if ( _parentTab )		// Is this view embedded into a tab widget?
    {
	if ( _parentTab->currentPage() == this )  // Is this page the topmost?
	{
	    showDetails( selectable );
	}
    }
    else	// No tab parent - simply show data unconditionally.
    {
	showDetails( selectable );
    }
}


QSize
YQPkgGenericDetailsView::minimumSizeHint() const
{
    return QSize( 0, 0 );
}


QString
YQPkgGenericDetailsView::htmlHeading( ZyppSel selectable, bool showVersion )
{
    if ( ! selectable )
	return "";

    ZyppObj zyppObj = selectable->theObj();

    if ( ! zyppObj )
	return "";

    QString summary = fromUTF8( zyppObj->summary() );

    QString html = "<table";

    if ( ! YQUI::ui()->usingVisionImpairedPalette() )
	html += " bgcolor=#E0E0F8";

    html += "><tr><td><b>"
	+ fromUTF8( zyppObj->name() )
	+ "</b>";

    if ( showVersion )
	html += QString( "<b>-" ) + zyppObj->edition().asString().c_str() + "</b>";

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


QString
YQPkgGenericDetailsView::table( const QString & contents )
{
    QString html = "<table border=1";

    if ( ! YQUI::ui()->usingVisionImpairedPalette() )
	html += " bgcolor=#F0F0F0";
    html += ">" + contents + "</table>";

    return html;
}


QString
YQPkgGenericDetailsView::row( const QString & contents )
{
    return "<tr>" + contents + "</tr>";
}


QString
YQPkgGenericDetailsView::cell( QString contents )
{
    contents = htmlEscape( contents );
    return "<td valign=top>" + contents + "</td>";
}


QString
YQPkgGenericDetailsView::cell( int contents )
{
    QString html;
    html.sprintf( "<td valign=top>%d</td>", contents );

    return html;
}


QString
YQPkgGenericDetailsView::cell( const zypp::Date & date )
{
    return cell( ( (time_t) date == (time_t) 0 ? "" : date.asString() ) );
}


QString
YQPkgGenericDetailsView::cell( const string & contents )
{
    return cell( fromUTF8( contents ) );
}


QString
YQPkgGenericDetailsView::hcell( QString contents )
{
    QString html = "<td valign=top";

    if ( ! YQUI::ui()->usingVisionImpairedPalette() )
	html += " bgcolor=#D0D0D0";

    html += ">" + contents + "</td>";

    return html;
}


#include "YQPkgGenericDetailsView.moc"
