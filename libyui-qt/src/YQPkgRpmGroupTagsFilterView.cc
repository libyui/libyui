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

  File:	      YQPkgRpmGroupTagsFilterView.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#define y2log_component "qt-pkg"
#include <ycp/y2log.h>
#include <qpixmap.h>
#include <qheader.h>

#include <Y2PM.h>
#include <y2pm/PMManager.h>

#include "YQPkgRpmGroupTagsFilterView.h"
#include "YUIQt.h"
#include "YQi18n.h"



YQPkgRpmGroupTagsFilterView::YQPkgRpmGroupTagsFilterView( YUIQt *yuiqt, QWidget *parent )
    : QListView( parent )
    , yuiqt(yuiqt)
{
    y2milestone( "Creating group tags view" );
    setFont( yuiqt->currentFont() );

    addColumn( _( "Package Groups" ) );
    setRootIsDecorated( true );
    cloneTree( Y2PM::packageManager().rpmGroupsTree()->root(), 0 );

    connect( this, SIGNAL( selectionChanged        ( QListViewItem * ) ),
	     this, SLOT  ( selectionChangedInternal( QListViewItem * ) ) );
}


YQPkgRpmGroupTagsFilterView::~YQPkgRpmGroupTagsFilterView()
{
    // NOP
}



void
YQPkgRpmGroupTagsFilterView::cloneTree( YStringTreeItem * 	parentRpmGroup,
					YQPkgRpmGroupTag * 	parentClone )
{
    YStringTreeItem * 	child = parentRpmGroup->firstChild();
    YQPkgRpmGroupTag * 	clone;

    while ( child )
    {
	if ( parentClone )
	    clone = new YQPkgRpmGroupTag( this, parentClone, child );
	else
	    clone = new YQPkgRpmGroupTag( this, child );

	CHECK_PTR( clone );
	clone->setOpen( clone->depth() < 1 );

	cloneTree( child, clone );
	child = child->next();
    }
}


void
YQPkgRpmGroupTagsFilterView::selectionChangedInternal( QListViewItem * sel )
{
    _selection = (YQPkgRpmGroupTag *) sel;
    filter();
}


void
YQPkgRpmGroupTagsFilterView::filterIfVisible()
{
    if ( isVisible() )
	filter();
}


void
YQPkgRpmGroupTagsFilterView::filter()
{
    emit filterStart();

    if ( _selection )
    {
	std::string selectedRpmGroupPath = Y2PM::packageManager().rpmGroup( _selection->rpmGroup() );
	y2debug( "Searching packages that match '%s'", selectedRpmGroupPath.c_str() );
	
	PMManager::PMSelectableVec::const_iterator it = Y2PM::packageManager().begin();

	while ( it != Y2PM::packageManager().end() )
	{
	    PMSelectablePtr selectable = *it;
	    
	    // Multiple instances of this package may or may not be in the same
	    // RPM group, so let's check both the installed version (if there
	    // is any) and the candidate version.
	    //
	    // Make sure we emit only one filterMatch() signal if both exist
	    // and both are in the same RPM group. We don't want multiple list
	    // entries for the same package!
	    
	    bool match =
		check( selectable->installedObj() ) || 
		check( selectable->candidateObj() );  

	    // If there is neither an installed nor a candidate package, check
	    // any other instance.  
	    
	    if ( ! match			&&
		 ! selectable->installedObj()	&&
		 ! selectable->candidateObj()     )
		check( selectable->theObject() );

	    ++it;
	}

	y2debug( "Search for '%s' finished", selectedRpmGroupPath.c_str() );
    }

    emit filterFinished();
}


bool
YQPkgRpmGroupTagsFilterView::check( PMPackagePtr pkg )
{
    if ( ! pkg || ! _selection )
	return false;

    if ( pkg->group_ptr() == 0 )
    {
	y2error( "NULL pointer in group_ptr()!" );
	return false;
    }

    if ( pkg->group_ptr()->isChildOf( _selection->rpmGroup() ) )
    {
#if 0
	// DEBUG
	std::string name = pkg->name();
	y2debug( "Found match for pkg '%s'", name.c_str() );
	// DEBUG
#endif
	
	emit filterMatch( pkg );
	return true;
    }
    else
    {
	return false;
    }
}





YQPkgRpmGroupTag::YQPkgRpmGroupTag( YQPkgRpmGroupTagsFilterView * 	parentFilterView,
				    YStringTreeItem *			rpmGroup	)
    : QListViewItem( parentFilterView )
    , _filterView( parentFilterView )
    , _rpmGroup( rpmGroup )
{
    setText( 0,  QString::fromUtf8( _rpmGroup->value().translation().c_str() ) );
}


YQPkgRpmGroupTag::YQPkgRpmGroupTag( YQPkgRpmGroupTagsFilterView * 	parentFilterView,
				    YQPkgRpmGroupTag * 			parentGroupTag,
				    YStringTreeItem *	 		rpmGroup	)
    : QListViewItem( parentGroupTag )
    , _filterView( parentFilterView )
    , _rpmGroup( rpmGroup )
{
    setText( 0,  QString::fromUtf8( _rpmGroup->value().translation().c_str() )  );
}


YQPkgRpmGroupTag::~YQPkgRpmGroupTag()
{
    // NOP
}



#include "YQPkgRpmGroupTagsFilterView.moc.cc"
