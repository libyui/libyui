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

  File:		YQPkgChangeLogView.cc

  Author:	Stefan Hundhammer <sh@suse.de>

  Textdomain	"qt-pkg"

/-*/

#define YUILogComponent "qt-pkg"
#include "YUILog.h"

#include <qregexp.h>
#include "YQPkgChangeLogView.h"
#include "YQPkgDescriptionDialog.h"
#include "YQi18n.h"
#include "utf8.h"



YQPkgChangeLogView::YQPkgChangeLogView( QWidget * parent )
    : YQPkgGenericDetailsView( parent )
{
}


YQPkgChangeLogView::~YQPkgChangeLogView()
{
    // NOP
}


void
YQPkgChangeLogView::showDetails( ZyppSel selectable )
{
    _selectable = selectable;

    if ( ! selectable )
    {
	clear();
	return;
    }

    QString html = htmlStart();
    html += htmlHeading( selectable, false );

    ZyppPkg installed = tryCastToZyppPkg( selectable->installedObj() );

    if ( installed )
    {
	html += changeLogTable( installed->changelog() );
    }
    else
    {
	html += "<p><i>" + _( "Information only available for installed packages." ) + "</i></p>";
    }
    html += htmlEnd();

    setHtml( html );
}



QString YQPkgChangeLogView::changeLogTable( const zypp::Changelog & changeLog ) const
{
    QString html;

    for ( zypp::Changelog::const_iterator it = changeLog.begin();
	  it != changeLog.end();
	  ++it )
    {
	QString changes = htmlEscape( fromUTF8( (*it).text() ) );
	changes.replace( "\n", "<br>" );
	changes.replace( " ", "&nbsp;" );

	html += row(
		    cell( (*it).date()   ) +
		    cell( (*it).author() ) +
		    "<td valign='top'>" + changes + "</td>" // cell() calls htmlEscape() !
		    );
    }

    return html.isEmpty() ? "" : table( html );
}


#include "YQPkgChangeLogView.moc"
