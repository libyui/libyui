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

#define USE_TACKAT_ICONS 0

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
    , YPkgRpmGroupTagsFilterView()
    , yuiqt(yuiqt)
{
    y2milestone( "Creating group tags view" );
    setFont( yuiqt->currentFont() );

    addColumn( _( "Package Groups" ) );
    setRootIsDecorated( true );
    cloneTree( root(), 0 );

    connect( this, SIGNAL( selectionChanged        ( QListViewItem * ) ),
	     this, SLOT  ( selectionChangedInternal( QListViewItem * ) ) );
}


YQPkgRpmGroupTagsFilterView::~YQPkgRpmGroupTagsFilterView()
{
    // NOP
}



void
YQPkgRpmGroupTagsFilterView::cloneTree( YPkgStringTreeItem * 	parentRpmGroup,
					YQPkgRpmGroupTag * 	parentClone )
{
    YPkgStringTreeItem * 	child = parentRpmGroup->firstChild();
    YQPkgRpmGroupTag *	 	clone;

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
	std::string selectedRpmGroup = completePath( _selection->rpmGroup(), '/',
						     false ); // startWithDelimiter
	
	y2debug( "Searching packages that match '%s'", selectedRpmGroup.c_str() );
	
	int i;
	PMManager::PMSelectableVec::const_iterator it;

	for ( it = Y2PM::packageManager().begin()	, i = 0;
	      it != Y2PM::packageManager().end()
		  // DEBUG
		  // DEBUG
		  // DEBUG
		  && i < 100;
		  // DEBUG
		  // DEBUG
		  // DEBUG
	      ++it					, i++ )
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
		check( selectable->installedObj(), selectedRpmGroup ) ||
		check( selectable->candidateObj(), selectedRpmGroup );

	    // If there is neither an installed nor a candidate package, check
	    // any other instance.  
	    
	    if ( ! match			&&
		 ! selectable->installedObj()	&&
		 ! selectable->candidateObj()     )
		check( selectable->theObject(), selectedRpmGroup );
	}

	y2debug( "Search for '%s' finished", selectedRpmGroup.c_str() );
    }

    emit filterFinished();
}


bool
YQPkgRpmGroupTagsFilterView::check( PMPackagePtr pkg, const std::string & selectedRpmGroup )
{
    if ( ! pkg )
	return false;


    if ( pkg->group().compare( 0, selectedRpmGroup.length(), selectedRpmGroup ) == 0 )
    {
#if 1
	// DEBUG
	std::string name = pkg->name();
	y2debug( "Found match for pkg '%s'", name.c_str() );
	// DEBUG
#endif
	emit filterMatch( pkg );
	
	return true;
    }

    return false;
}





YQPkgRpmGroupTag::YQPkgRpmGroupTag( YQPkgRpmGroupTagsFilterView * 	parentFilterView,
				    YPkgStringTreeItem *		rpmGroup	)
    : QListViewItem( parentFilterView )
    , _filterView( parentFilterView )
    , _rpmGroup( rpmGroup )
{
    setText( 0,  QString::fromUtf8( _rpmGroup->value().translation().c_str() ) );
}


YQPkgRpmGroupTag::YQPkgRpmGroupTag( YQPkgRpmGroupTagsFilterView * 	parentFilterView,
				    YQPkgRpmGroupTag * 			parentGroupTag,
				    YPkgStringTreeItem * 		rpmGroup	)
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
