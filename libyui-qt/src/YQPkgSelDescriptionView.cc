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

#include "YQZypp.h"
#include <zypp/Selection.h>

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
YQPkgSelDescriptionView::showDetails( zypp::ui::Selectable::Ptr selectable )
{
    _selectable = selectable;
    
    if ( ! selectable )
    {
	clear();
	return;
    }

    QString html_text = htmlHeading( selectable );

    QString description = fromUTF8( selectable->theObj()->description() );

    if ( ! description.contains( "<!-- DT:Rich -->" ) )
	description = simpleHtmlParagraphs( description );

    html_text += description;

    setTextFormat( Qt::RichText );
    setText( html_text );
    ensureVisible( 0, 0 );	// Otherwise hyperlinks will be centered
}



QString
YQPkgSelDescriptionView::htmlHeading( zypp::ui::Selectable::Ptr selectable )
{
    zypp::Selection::constPtr sel =
	zypp::dynamic_pointer_cast<const zypp::Selection>( selectable->theObj() );

    if ( ! sel )
	return YQPkgGenericDetailsView::htmlHeading( selectable );
    
    QString summary = fromUTF8( sel->summary() );
    bool useBigFont = ( summary.length() <= 40 );

    if ( summary.isEmpty() )
	summary = fromUTF8( sel->name() );

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
