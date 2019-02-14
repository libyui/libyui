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
#include <yui/Libyui_config.h>

#include <unistd.h>
#include <QRegExp>

#include "YQPkgSelDescriptionView.h"
#include "YQUI.h"
#include "YQi18n.h"
#include "utf8.h"
#include <qbuffer.h>

using std::endl;
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

    QString iconName = pattern ? pattern->icon().asString().c_str() : "";

    if ( iconName.isEmpty() )
    {
	iconName = zyppObj->name().c_str();
	iconName.replace( ' ', '_' );
    }

    if ( ! iconName.isEmpty() )
    {
	if ( iconName.startsWith( "./" ) )
	    iconName.replace( QRegExp( "^\\./" ), "" );

	if ( pattern && iconName.isEmpty() )
	    yuiWarning() << "No icon for pattern " << zyppObj->name() << endl;
    }


    QString html = "<table width='100%'><tr><td>"
	+ ( useBigFont ? QString( "<h2>" ) : QString( "<b>" ) )
	+ summary
	+ ( useBigFont ? QString( "</h2>" ) : QString( "</b>" ) )
	+ "</td></tr>"
	+ "</table>";

    if ( ! iconName.isEmpty() )
    {
	QIcon icon = YQUI::ui()->loadIcon( iconName.toStdString() );
        QPixmap pixmap = icon.pixmap(16);
        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        pixmap.save(&buffer, "PNG");
	html = QString( "<table width='100%'><tr>" )
	    + "<td><img src=\"data:image/png;base64," + byteArray.toBase64() + "\"/></td>"
	    + "<td width='100%'>" + html + "</td>"
	    + "</tr></table>";
    }

    return html;
}
