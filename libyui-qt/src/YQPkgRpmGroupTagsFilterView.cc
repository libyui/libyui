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

  Textdomain "packages-qt"

/-*/


#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include "YQZypp.h"
#include <zypp/ui/ResPoolProxy.h>

#include "YQPkgRpmGroupTagsFilterView.h"
#include "YQi18n.h"
#include "utf8.h"



YQPkgRpmGroupTagsFilterView::YQPkgRpmGroupTagsFilterView( QWidget * parent )
    : QListView( parent )
{
    addColumn( _( "Package Groups" ) );
    setRootIsDecorated( true );
    cloneTree( Y2PM::packageManager().rpmGroupsTree()->root(), 0 );
    new YQPkgRpmGroupTag( this, _( "zzz All" ), 0 );

    connect( this, SIGNAL( selectionChanged( QListViewItem * ) ),
	     this, SLOT  ( filter()                            ) );

    selectSomething();
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
YQPkgRpmGroupTagsFilterView::selectSomething()
{
    QListViewItem * item = firstChild();

    if ( item )
    {
	setSelected( item, true );
    }
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

    if ( selection() )
    {
	PMManager::SelectableVec::const_iterator it = Y2PM::packageManager().begin();

	while ( it != Y2PM::packageManager().end() )
	{
	    Selectable::Ptr selectable = *it;

	    // Multiple instances of this package may or may not be in the same
	    // RPM group, so let's check both the installed version (if there
	    // is any) and the candidate version.
	    //
	    // Make sure we emit only one filterMatch() signal if both exist
	    // and both are in the same RPM group. We don't want multiple list
	    // entries for the same package!

	    bool match =
		check( selectable->candidateObj() ) ||
		check( selectable->installedObj() );

	    // If there is neither an installed nor a candidate package, check
	    // any other instance.

	    if ( ! match			&&
		 ! selectable->candidateObj()   &&
		 ! selectable->installedObj()	  )
		check( selectable->theObj() );

	    ++it;
	}
    }

    emit filterFinished();
}


bool
YQPkgRpmGroupTagsFilterView::check( zypp::Package::constPtr pkg )
{
    if ( ! pkg || ! selection() )
	return false;

    if ( selection()->rpmGroup() == 0 )	// Special case: All packages
    {
	emit filterMatch( pkg );
	return true;
    }


    if ( pkg->group_ptr() == 0 )
    {
	y2error( "NULL pointer in group_ptr() for package %s", pkg->name().asString().c_str() );
	return false;
    }

    if ( pkg->group_ptr()->isChildOf( selection()->rpmGroup() ) )
    {
	emit filterMatch( pkg );
	return true;
    }

    return false;
}


YQPkgRpmGroupTag *
YQPkgRpmGroupTagsFilterView::selection() const
{
    QListViewItem * item = selectedItem();

    if ( ! item )
	return 0;

    return dynamic_cast<YQPkgRpmGroupTag *> ( selectedItem() );
}






YQPkgRpmGroupTag::YQPkgRpmGroupTag( YQPkgRpmGroupTagsFilterView * 	parentFilterView,
				    YStringTreeItem *			rpmGroup	)
    : QListViewItem( parentFilterView )
    , _filterView( parentFilterView )
    , _rpmGroup( rpmGroup )
{
    setText( 0,  fromUTF8( _rpmGroup->value().translation().c_str() ) );
}


YQPkgRpmGroupTag::YQPkgRpmGroupTag( YQPkgRpmGroupTagsFilterView * 	parentFilterView,
				    YQPkgRpmGroupTag * 			parentGroupTag,
				    YStringTreeItem *	 		rpmGroup	)
    : QListViewItem( parentGroupTag )
    , _filterView( parentFilterView )
    , _rpmGroup( rpmGroup )
{
    setText( 0,  fromUTF8( _rpmGroup->value().translation().c_str() )  );
}


YQPkgRpmGroupTag::YQPkgRpmGroupTag( YQPkgRpmGroupTagsFilterView * 	parentFilterView,
				    const QString &			rpmGroupName,
				    YStringTreeItem *			rpmGroup	)
    : QListViewItem( parentFilterView )
    , _filterView( parentFilterView )
    , _rpmGroup( rpmGroup )
{
    setText( 0,  rpmGroupName );
}


YQPkgRpmGroupTag::~YQPkgRpmGroupTag()
{
    // NOP
}



#include "YQPkgRpmGroupTagsFilterView.moc"
