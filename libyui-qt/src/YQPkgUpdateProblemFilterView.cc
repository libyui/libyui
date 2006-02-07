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

#include "YQZypp.h"
#include <zypp/ui/ResPoolProxy.h>

#include "YQPkgUpdateProblemFilterView.h"
#include "YQi18n.h"
#include "utf8.h"



YQPkgUpdateProblemFilterView::YQPkgUpdateProblemFilterView( QWidget * parent )
    : QTextBrowser( parent )
{
    QString html = _( "\
<br>\n\
<h2>Update Problem</h2>\n\
<p>\n\
<font color=blue>\n\
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

#ifdef MISSING
    PMManager::SelectableVec::const_iterator it = Y2PM::packageManager().updateBegin();

    while ( it != Y2PM::packageManager().updateEnd() )
    {
	Selectable::Ptr selectable = *it;
	emit filterMatch( *it, ( *it)->theObj() );

	++it;
    }
#endif

    emit filterFinished();
}


#include "YQPkgUpdateProblemFilterView.moc"
