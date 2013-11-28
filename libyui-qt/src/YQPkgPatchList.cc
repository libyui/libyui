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

  File:	      YQPkgPatchList.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  Textdomain "packages-qt"

/-*/


#define y2log_component "qt-pkg"
#include <ycp/y2log.h>

#include <qpopupmenu.h>
#include <qaction.h>
#include <zypp/Patch.h>
#include <set>

#include "YQi18n.h"
#include "utf8.h"

#include "YQPkgPatchList.h"
#include "YQPkgTextDialog.h"


#define VERBOSE_PATCH_LIST	1

using std::list;
using std::set;

YQPkgPatchList::YQPkgPatchList( QWidget * parent )
    : YQPkgObjList( parent )
{
    y2debug( "Creating patch list" );

    _filterCriteria = RelevantPatches;

    int numCol = 0;
    addColumn( "" );			_statusCol	= numCol++;
    addColumn( _( "Patch"	) );	_nameCol	= numCol++;
    addColumn( _( "Summary" 	) );	_summaryCol	= numCol++;
    addColumn( _( "Category"	) );	_categoryCol	= numCol++;
    addColumn( _( "Size"	) );	_sizeCol	= numCol++;
    addColumn( _( "Version"	) );	_versionCol	= numCol++;

    // Can use the same colum for "broken" and "satisfied":
    // Both states are mutually exclusive

    _satisfiedIconCol	= _summaryCol;
    _brokenIconCol	= _summaryCol;

    setAllColumnsShowFocus( true );
    setColumnAlignment( sizeCol(), Qt::AlignRight );

    connect( this,	SIGNAL( selectionChanged	( QListViewItem * ) ),
	     this,	SLOT  ( filter()				    ) );

    setSorting( categoryCol() );
    fillList();

    y2debug( "Creating patch list done" );
}


YQPkgPatchList::~YQPkgPatchList()
{
    // NOP
}


void
YQPkgPatchList::polish()
{
    // Delayed initialization after widget is fully created etc.

    // Only now send selectionChanged() signal so attached details views also
    // display something if their showDetailsIfVisible() slot is connected to
    // selectionChanged() signals.
    selectSomething();
}


void
YQPkgPatchList::setFilterCriteria( FilterCriteria filterCriteria )
{
    _filterCriteria = filterCriteria;
}

void
YQPkgPatchList::fillList()
{
  // wee need to do a full solve in order
  // to get the satisfied status correctly
    
  clear();
  y2debug("Filling patch list");


  for ( ZyppPoolIterator it = zyppPatchesBegin();
	it != zyppPatchesEnd();
	++it )
    {
      ZyppSel  selectable = *it;
      ZyppPatch zyppPatch = tryCastToZyppPatch( selectable->theObj() );

      if ( zyppPatch )
        {
	  bool displayPatch = false;

	  switch ( _filterCriteria )
            {
            case RelevantPatches:// needed + broken + satisfied (but not installed)
                
	      // only shows patches relevant to the system
	      if ( selectable->hasCandidateObj() && 
		   selectable->candidateObj().isRelevant() )
                {
		  // and only those that are needed
		  if ( ! selectable->candidateObj().isSatisfied() ||
		       // may be it is satisfied because is preselected
		       selectable->candidateObj().status().isToBeInstalled() )
		    displayPatch = true;
		  else
		    y2debug( "Patch %s is already satisfied", zyppPatch->ident().c_str());
                }
	      else
		y2debug( "Patch %s is not relevant to the system", zyppPatch->ident().c_str());
	      break;
            case RelevantAndInstalledPatches:// patches we dont need
                
	      // only shows patches relevant to the system
	      if ( ( selectable->hasCandidateObj() ) && 
		   ( ! selectable->candidateObj().isRelevant() 
		     || ( selectable->candidateObj().isSatisfied() &&
			  ! selectable->candidateObj().status().isToBeInstalled() ) ) )
                {
		  // now we show satisfied patches too
		  displayPatch = true;
                }
	      break;
            case AllPatches:
	      displayPatch = true;
	      break;
                
	      // Intentionally omitting "default" so the compiler
	      // can catch unhandled enum values
            default:
	      y2debug("unknown patch filter");
                
            }
            
	  if ( displayPatch )
            {
#if VERBOSE_PATCH_LIST
		y2debug( "Displaying patch %s - %s", zyppPatch->name().c_str(), zyppPatch->summary().c_str() );
#endif

	      addPatchItem( *it, zyppPatch);
            }
        }
      else
        {
	  y2error("Found non-patch selectable");
        }
    }
    
  y2debug("Patch list filled");
  //resizeColumnToContents(_statusCol);
  //resizeColumnToContents(_nameCol);
  //resizeColumnToContents(_categoryCol);
}

void
YQPkgPatchList::message( const QString & text )
{
    QY2ListViewItem * item = new QY2ListViewItem( this );
    CHECK_PTR( item );

    item->setText( 1, text );
    item->setBackgroundColor( QColor( 0xE0, 0xE0, 0xF8 ) );
}



void
YQPkgPatchList::filterIfVisible()
{
    if ( isVisible() )
	filter();
}


void
YQPkgPatchList::filter()
{
  emit filterStart();

  if ( selection() )
    {
      ZyppPatch patch = selection()->zyppPatch();

      if ( patch )
        {
	  zypp::Patch::Contents contents(patch->contents());
	  //y2milestone(contents.asString().c_str());
            
	  for ( zypp::Patch::Contents::Selectable_iterator it = contents.selectableBegin();
		it != contents.selectableEnd();
		++it )
            {
	      ZyppPkg zyppPkg = tryCastToZyppPkg( (*it)->theObj() );
	      if ( zyppPkg )
                {
		  emit filterMatch( *it, zyppPkg );
                }
            }
        }
      else
        {
	  y2milestone("patch is bogus");
        }
        
    }
  else
    y2warning("selection empty");

  emit filterFinished();
}

void
YQPkgPatchList::addPatchItem( ZyppSel	selectable,
			      ZyppPatch zyppPatch )
{
    if ( ! selectable )
    {
	y2error( "NULL ZyppSel!" );
	return;
    }

    new YQPkgPatchListItem( this, selectable, zyppPatch );
}


YQPkgPatchListItem *
YQPkgPatchList::selection() const
{
    QListViewItem * item = selectedItem();

    if ( ! item )
	return 0;

    return dynamic_cast<YQPkgPatchListItem *> (item);
}


void
YQPkgPatchList::createNotInstalledContextMenu()
{
    _notInstalledContextMenu = new QPopupMenu( this );
    CHECK_PTR( _notInstalledContextMenu );

    actionSetCurrentInstall->addTo( _notInstalledContextMenu );
    actionSetCurrentDontInstall->addTo( _notInstalledContextMenu );
    actionSetCurrentTaboo->addTo( _notInstalledContextMenu );

    addAllInListSubMenu( _notInstalledContextMenu );
}


void
YQPkgPatchList::createInstalledContextMenu()
{
    _installedContextMenu = new QPopupMenu( this );
    CHECK_PTR( _installedContextMenu );

    actionSetCurrentKeepInstalled->addTo( _installedContextMenu );

#if ENABLE_DELETING_PATCHES
    actionSetCurrentDelete->addTo( _installedContextMenu );
#endif

    actionSetCurrentUpdate->addTo( _installedContextMenu );
    actionSetCurrentProtected->addTo( _installedContextMenu );

    addAllInListSubMenu( _installedContextMenu );
}


QPopupMenu *
YQPkgPatchList::addAllInListSubMenu( QPopupMenu * menu )
{
    QPopupMenu * submenu = new QPopupMenu( menu );
    CHECK_PTR( submenu );

    actionSetListInstall->addTo( submenu );
    actionSetListDontInstall->addTo( submenu );
    actionSetListKeepInstalled->addTo( submenu );

#if ENABLE_DELETING_PATCHES
    actionSetListDelete->addTo( submenu );
#endif

    actionSetListUpdate->addTo( submenu );
    actionSetListUpdateForce->addTo( submenu );
    actionSetListTaboo->addTo( submenu );
    actionSetListProtected->addTo( submenu );

    menu->insertItem( _( "&All in This List" ), submenu );

    return submenu;
}


void
YQPkgPatchList::keyPressEvent( QKeyEvent * event )
{
    if ( event )
    {
#if ! ENABLE_DELETING_PATCHES
	if ( event->ascii() == '-' )
	{
	    QListViewItem * selectedListViewItem = selectedItem();

	    if ( selectedListViewItem )
	    {
		YQPkgPatchListItem * item = dynamic_cast<YQPkgPatchListItem *> (selectedListViewItem);

		if ( item && item->selectable()->hasInstalledObj() )
		{
		    y2warning( "Deleting patches is not supported" );
		    return;
		}
	    }
	}
#endif
    }

    YQPkgObjList::keyPressEvent( event );
}




YQPkgPatchListItem::YQPkgPatchListItem( YQPkgPatchList *	patchList,
					ZyppSel			selectable,
					ZyppPatch		zyppPatch )
    : YQPkgObjListItem( patchList, selectable, zyppPatch )
    , _patchList( patchList )
    , _zyppPatch( zyppPatch )
{
    if ( ! _zyppPatch )
	_zyppPatch = tryCastToZyppPatch( selectable->theObj() );

    if ( ! _zyppPatch )
	return;

    setStatusIcon();
    _patchCategory = patchCategory( _zyppPatch->category() );

    if ( categoryCol() > -1 )
	setText( categoryCol(), asString( _patchCategory ) );

    if ( summaryCol() > -1 && _zyppPatch->summary().empty() )
	setText( summaryCol(), _zyppPatch->name() );		// use name as fallback

    switch ( _patchCategory )
    {
	case YQPkgYaSTPatch:		setTextColor( QColor( 0, 0, 0xC0 ) );	break;	// medium blue
	case YQPkgSecurityPatch:	setTextColor( Qt::red );		break;
	case YQPkgRecommendedPatch:	setTextColor( QColor( 0, 0, 0xC0 ) );	break;	// medium blue
	case YQPkgOptionalPatch:	break;
	case YQPkgDocumentPatch:	break;
	case YQPkgUnknownPatchCategory: break;
    }
}


YQPkgPatchListItem::~YQPkgPatchListItem()
{
    // NOP
}


YQPkgPatchCategory
YQPkgPatchListItem::patchCategory( QString category )
{
    category = category.lower();

    if ( category == "yast"		) return YQPkgYaSTPatch;
    if ( category == "security"		) return YQPkgSecurityPatch;
    if ( category == "recommended"	) return YQPkgRecommendedPatch;
    if ( category == "optional"		) return YQPkgOptionalPatch;
    if ( category == "document"		) return YQPkgDocumentPatch;

    y2warning( "Unknown patch category \"%s\"", (const char *) category );
    return YQPkgUnknownPatchCategory;
}


QString
YQPkgPatchListItem::asString( YQPkgPatchCategory category )
{
    switch ( category )
    {
	// Translators: These are patch categories
	case YQPkgYaSTPatch:		return _( "YaST"	);
	case YQPkgSecurityPatch:	return _( "security"	);
	case YQPkgRecommendedPatch:	return _( "recommended" );
	case YQPkgOptionalPatch:	return _( "optional"	);
	case YQPkgDocumentPatch:	return _( "document"	);
	case YQPkgUnknownPatchCategory: return "";
    }

    return "";
}


void
YQPkgPatchListItem::cycleStatus()
{
    YQPkgObjListItem::cycleStatus();

#if ! ENABLE_DELETING_PATCHES
    if ( status() == S_Del )	// Can't delete patches
	setStatus( S_KeepInstalled );
#endif
}


QString
YQPkgPatchListItem::toolTip( int col )
{
    QString text;

    if ( col == statusCol() )
    {
	text = YQPkgObjListItem::toolTip( col );
    }
    else
    {
	if (  ( col == brokenIconCol()	  && isBroken()	   ) ||
	      ( col == satisfiedIconCol() && isSatisfied() )   )
	{
	    text = YQPkgObjListItem::toolTip( col );
	}
	else
	{
	    text = fromUTF8( zyppPatch()->category() );

	    if ( ! text.isEmpty() )
		text += "\n";

	    text += fromUTF8( zyppPatch()->downloadSize().asString().c_str() );
	}
    }

    return text;
}


void
YQPkgPatchListItem::applyChanges()
{
    solveResolvableCollections();
}


/**
 * Comparison function used for sorting the list.
 * Returns:
 * -1 if this <	 other
 *  0 if this == other
 * +1 if this >	 other
 **/
int
YQPkgPatchListItem::compare( QListViewItem *	otherListViewItem,
			     int		col,
			     bool		ascending ) const
{
    YQPkgPatchListItem * other = dynamic_cast<YQPkgPatchListItem *> (otherListViewItem);

    if ( other )
    {
	if ( col == categoryCol() )
	{
	    if ( this->patchCategory() < other->patchCategory() ) return -1;
	    if ( this->patchCategory() > other->patchCategory() ) return  1;
	    return 0;
	}
    }
    return YQPkgObjListItem::compare( otherListViewItem, col, ascending );
}



#include "YQPkgPatchList.moc"
