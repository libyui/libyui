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

  Textdomain "packages-qt"

/-*/


#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include <Y2PM.h>
#include <y2pm/PMManager.h>

#include "YQPkgUpdateProblemFilterView.h"
#include "YQi18n.h"
#include "utf8.h"



YQPkgUpdateProblemFilterView::YQPkgUpdateProblemFilterView( QWidget *parent )
    : QTextBrowser( parent )
{
    y2milestone( "Creating update problems view" );

    QString html = _( "\
<br>\
<h2>Update Problem</h2>\
<p>\
<font color=blue>\
The packages in this list cannot be updated automatically.\
</font>\
</p>\
<p>Possible reasons:</p>\
<ul>\
<li>They are obsoleted by other packages\
<li>There is no newer version to update to on any installation media\
<li>They are third-party packages\
</ul>\
</p>\
<p>\
Please choose manually what to do with them.\
The safest course of action is to delete them.\
</p>\
" );

    setTextFormat( Qt::RichText );
    setText( html );
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

    PMManager::PMSelectableVec::const_iterator it = Y2PM::packageManager().updateBegin();

    while ( it != Y2PM::packageManager().updateEnd() )
    {
	PMSelectablePtr selectable = *it;
	emit filterMatch( (*it)->theObject() );
	    
	++it;
    }

    emit filterFinished();
}


#include "YQPkgUpdateProblemFilterView.moc.cc"
