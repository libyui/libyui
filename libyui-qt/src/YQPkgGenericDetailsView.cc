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
    _zyppObj = 0;
    _parentTab = dynamic_cast<QTabWidget *> (parent);

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
	showDetailsIfVisible( _zyppObj );
    }
}


void
YQPkgGenericDetailsView::showDetailsIfVisible( zypp::ResObject::constPtr zyppObj )
{
    _zyppObj = zyppObj;

    if ( _parentTab )		// Is this view embedded into a tab widget?
    {
	if ( _parentTab->currentPage() == this )  // Is this page the topmost?
	{
	    showDetails( zyppObj );
	}
    }
    else	// No tab parent - simply show data unconditionally.
    {
	showDetails( zyppObj );
    }
}


QSize
YQPkgGenericDetailsView::minimumSizeHint() const
{
    return QSize( 0, 0 );
}


QString
YQPkgGenericDetailsView::htmlHeading( zypp::ResObject::constPtr zyppObj )
{
    QString summary = fromUTF8( zyppObj->summary() );

    QString html = "<table";

    if ( ! YQUI::ui()->usingVisionImpairedPalette() )
	html += " bgcolor=#E0E0F8";

    html += "><tr><td><b>"
	+ fromUTF8( zyppObj->name() )
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
    return "<td align=top>" + contents + "</td>";
}


QString
YQPkgGenericDetailsView::cell( int contents )
{
    QString html;
    html.sprintf( "<td align=top>%d</td>", contents );

    return html;
}


QString
YQPkgGenericDetailsView::cell( const Date & contents )
{
    return cell( formatDate( contents ) );
}


QString
YQPkgGenericDetailsView::cell( const string & contents )
{
    return cell( QString::fromUtf8( contents.c_str() ) );
}


QString
YQPkgGenericDetailsView::hcell( QString contents )
{
    QString html = "<td align=top";
    
    if ( ! YQUI::ui()->usingVisionImpairedPalette() )
	html += " bgcolor=#D0D0D0";
    
    html += ">" + contents + "</td>";

    return html;
}


QString
YQPkgGenericDetailsView::formatDate( const Date & weird_date )
{
    time_t seconds = weird_date;

    if ( seconds == 0 ) // Special case: Don't display "1.1.1970 0:00"
	return "";

    QDateTime date;
    date.setTime_t( seconds );

    return date.toString( Qt::LocalDate );
}



#include "YQPkgGenericDetailsView.moc"
