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

  File:	      YQPkgConflictList.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#define y2log_component "qt-pkg"
#include <ycp/y2log.h>
#include <Y2PM.h>
#include <y2pm/PkgDep.h>
#include <y2pm/PMObject.h>

#include "YQPkgConflictList.h"

#include "YQi18n.h"
#include "utf8.h"



YQPkgConflictList::YQPkgConflictList( QWidget * parent )
    : QY2ListView( parent )
{
    addColumn( _("Dependency conflict") );
}


YQPkgConflictList::~YQPkgConflictList()
{
    // NOP
}


void
YQPkgConflictList::fill( PkgDep::ErrorResultList & badList )
{
    clear();
    QY2ListViewItem * item;
    std::string text;
    
    std::list<PkgDep::ErrorResult>::iterator it = badList.begin();

    while ( it != badList.end() )
    {
	PMSolvablePtr solvable = (*it).solvable;
	PMObjectPtr pmObj = solvable;
	    
	item = new QY2ListViewItem( this );

	if ( solvable )
	{
	    if ( ! pmObj )
		item->setText( 0, "<Unspecified - no PMObject>" );
	    else
	    {
		text = pmObj->name();
		item->setText( 0, fromUTF8( text ) );
	    }
	}
	else
	{
	    item->setText( 0, "<Unspecified>" );
	}
	
	++it;
    }
}


bool
YQPkgConflictList::choicesComplete()
{
    // TODO
    return true;
}


void
YQPkgConflictList::activateUserChoices()
{
    // TODO
    // TODO
    // TODO
}



#include "YQPkgConflictList.moc.cc"
