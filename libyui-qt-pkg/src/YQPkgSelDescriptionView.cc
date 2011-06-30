/**************************************************************************
Copyright (C) 2000 - 2010 Novell, Inc.
All Rights Reserved.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

**************************************************************************/


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

#define YUILogComponent "qt-pkg"
#include "YUILog.h"

#include <unistd.h>
#include <QRegExp>

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

    setHtml( html_text );
    //FIXME ensureVisible( 0, 0 );	// Otherwise hyperlinks will be centered
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

    if ( ! icon.isEmpty() )
    {
	if ( icon.startsWith( "./" ) )
	    icon.replace( QRegExp( "^\\./" ), "" );

	if ( ! icon.endsWith( ".png", Qt::CaseInsensitive ) &&
	     ! icon.endsWith( ".jpg", Qt::CaseInsensitive )   )
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
	    yuiWarning() << "No icon for pattern " << zyppObj->name()
			 << " - icon name: " << origIconName
			 << endl;
    }


    QString html = "<table width='100%'";

    if ( ! YQUI::ui()->usingVisionImpairedPalette() )
	html += " bgcolor='"
	     + QApplication::palette().color( QPalette::Active, QPalette::Base).name()
             + "'";

    html += "><tr><td>"
	+ ( useBigFont ? QString( "<h2>" ) : QString( "<b>" ) )
	+ summary
	+ ( useBigFont ? QString( "</h2>" ) : QString( "</b>" ) )
	+ "</td></tr>"
	+ "</table>";

    if ( ! icon.isEmpty() )
    {
	html = QString( "<table width='100%'><tr>" )
	    + "<td><img src=\"" + icon + "\"></td>"
	    + "<td width='100%'>" + html + "</td>"
	    + "</tr></table>";
    }

    return html;
}



QString
YQPkgSelDescriptionView::findIcon( const QString & icon ) const
{
    if ( access( qPrintable( icon ), R_OK ) == 0 )
    {
	yuiDebug() << "Found icon " << icon << endl;
	return icon;
    }
    else
    {
	yuiDebug() << "No icon " << icon << endl;
	return "";
    }
}


#include "YQPkgSelDescriptionView.moc"
