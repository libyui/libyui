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

  File:	      YQPkgUpdateProblemFilterView.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  Textdomain "qt-pkg"

/-*/


#define YUILogComponent "qt-pkg"
#include "YUILog.h"

#include "YQPkgUpdateProblemFilterView.h"
#include "YQi18n.h"
#include "utf8.h"

#include <zypp/ZYppFactory.h>
#include <zypp/Resolver.h>

using std::list;


YQPkgUpdateProblemFilterView::YQPkgUpdateProblemFilterView( QWidget * parent )
    : QTextBrowser( parent )
{
    QString html = _( "\
<br>\n\
<h2>Update Problem</h2>\n\
<p>\n\
<font color='blue'>\n\
The packages in this list cannot be updated automatically.\n\
</font>\n\
</p>\n\
<p>Possible reasons:</p>\n\
<ul>\n\
<li>They are obsoleted by other packages\n\
<li>There is no newer version to update to on any installation media\n\
<li>They are third-party packages\n\
</ul>\n\
</p>\n\
<p>\n\
Please choose manually what to do with them.\n\
The safest course of action is to delete them.\n\
</p>\
" );

    setHtml( html );
}


YQPkgUpdateProblemFilterView::~YQPkgUpdateProblemFilterView()
{
    // NOP
}

void
YQPkgUpdateProblemFilterView::filterIfVisible()
{
    if ( isVisible() )
	filter();
}


void
YQPkgUpdateProblemFilterView::filter()
{
    emit filterStart();

    list<zypp::PoolItem> problemList = zypp::getZYpp()->resolver()->problematicUpdateItems();

    for ( list<zypp::PoolItem>::const_iterator it = problemList.begin();
	  it != problemList.end();
	  ++it )
    {
	ZyppPkg pkg = tryCastToZyppPkg( (*it).resolvable() );

	if ( pkg )
	{
	    ZyppSel sel = _selMapper.findZyppSel( pkg );

	    if ( sel )
	    {
		yuiMilestone() << "Problematic package: "
			       << pkg->name() << "-" << pkg->edition().asString()
			       << endl;

		emit filterMatch( sel, pkg );
	    }
	}

    }

    emit filterFinished();
}


bool
YQPkgUpdateProblemFilterView::haveProblematicPackages()
{
    return ! zypp::getZYpp()->resolver()->problematicUpdateItems().empty();
}


#include "YQPkgUpdateProblemFilterView.moc"
