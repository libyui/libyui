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

#include <unistd.h>
#include <qregexp.h>

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
YQPkgSelDescriptionView::showDetails( ZyppSel selectable )
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
YQPkgSelDescriptionView::htmlHeading( ZyppSel selectable )
{
    ZyppObj       	zyppObj       	= selectable->theObj();
    ZyppPattern		pattern		= tryCastToZyppPattern  ( zyppObj );

    if ( ! pattern )
	return YQPkgGenericDetailsView::htmlHeading( selectable );

    QString summary = fromUTF8( zyppObj->summary() );
    bool useBigFont = ( summary.length() <= 40 );

    if ( summary.isEmpty() )			// No summary?
	summary = fromUTF8( zyppObj->name() );	// Use name instead (internal only normally)

    QString icon = pattern ? pattern->icon().asString().c_str() : "";

    if ( icon.isEmpty() )
    {
	icon = zyppObj->name().c_str();
	icon.replace( ' ', '_' );
    }

    if ( icon )
    {
	if ( icon.startsWith( "./" ) )
	    icon.replace( QRegExp( "^\\./" ), "" );
	    
	if ( ! icon.endsWith( ".png", false ) &&
	     ! icon.endsWith( ".jpg", false )   )
	    icon += ".png";

	QString origIconName = icon;

	if ( ! icon.contains( "/" ) )		// no path at all
	{
	    // Look in icon directories:
	    //
	    //   /usr/share/YaST2/theme/current/icons/32x32/apps/
	    //   /usr/share/YaST2/theme/current/icons/48x48/apps/

	    QString iconBaseName = icon;
	    icon = findIcon( QString( THEMEDIR ) + "/icons/32x32/apps/" + iconBaseName );

	    if ( icon.isEmpty() )
		icon = findIcon( QString( THEMEDIR ) + "/icons/48x48/apps/" + iconBaseName );
	}
	else if ( ! icon.startsWith( "/" ) )	// relative path
	{
	    // Use path relative to theme directory:
	    //
	    //   /usr/share/YaST2/theme/current/ + icon

	    icon = findIcon( QString( THEMEDIR ) + "/" + icon );
	}

	if ( pattern && icon.isEmpty() )
	    y2warning( "No icon for pattern %s - icon name: %s",
		       zyppObj->name().c_str(), (const char *) origIconName );
    }


    QString html = "<table width=100%";

    if ( ! YQUI::ui()->usingVisionImpairedPalette() )
	html += " bgcolor=#C8C8F8";	// or #E0E0F8 (very light blueish grey)

    html += "><tr><td>"
	+ ( useBigFont ? QString( "<h2>" ) : QString( "<b>" ) )
	+ summary
	+ ( useBigFont ? QString( "</h2>" ) : QString( "</b>" ) )
	+ "</td></tr>"
	+ "</table>";

    if ( ! icon.isEmpty() )
    {
	html = QString( "<table width=100%><tr>" )
	    + "<td><img src=\"" + icon + "\"></td>"
	    + "<td width=100%>" + html + "</td>"
	    + "</tr></table>";
    }

    return html;
}



QString
YQPkgSelDescriptionView::findIcon( const QString & icon ) const
{
    if ( access( icon, R_OK ) == 0 )
    {
	y2debug( "Found icon %s", (const char *) icon );
	return icon;
    }
    else
    {
	y2debug( "No icon %s", (const char *) icon );
	return "";
    }
}


#include "YQPkgSelDescriptionView.moc"
