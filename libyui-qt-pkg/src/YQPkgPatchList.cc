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

#define YUILogComponent "qt-pkg"
#include "YUILog.h"

#include <QMenu>
#include <QAction>
#include <QEvent>
#include <zypp/ui/PatchContents.h>
#include <set>

#include "YQi18n.h"
#include "utf8.h"

#include "YQPkgPatchList.h"
#include "YQPkgTextDialog.h"


#define VERBOSE_PATCH_LIST	1


typedef zypp::ui::PatchContents			ZyppPatchContents;
typedef zypp::ui::PatchContents::const_iterator ZyppPatchContentsIterator;

using std::list;
using std::set;


YQPkgPatchList::YQPkgPatchList( QWidget * parent )
    : YQPkgObjList( parent )
{
    yuiDebug() << "Creating patch list" << endl;

    _filterCriteria = RelevantPatches;

    int numCol = 0;

    QStringList headers;

    headers << "";			_statusCol	= numCol++;
    headers <<  _( "Patch"	);	_nameCol	= numCol++;
    headers << _( "Summary" 	);	_summaryCol	= numCol++;
    headers << _( "Category"	);	_categoryCol	= numCol++;
    headers << _( "Size"	);	_sizeCol	= numCol++;
    headers << _( "Version"	);	_versionCol	= numCol++;

    // Can use the same colum for "broken" and "satisfied":
    // Both states are mutually exclusive

    _satisfiedIconCol	= _summaryCol;
    _brokenIconCol	= _summaryCol;

    setHeaderLabels(headers);

    setAllColumnsShowFocus( true );
    //FIXME setColumnAlignment( sizeCol(), Qt::AlignRight );

    connect( this,	SIGNAL( currentItemChanged	( QTreeWidgetItem *, QTreeWidgetItem* ) ),
	     this,	SLOT  ( filter()				    ) );

    sortItems( categoryCol(), Qt::AscendingOrder );
    fillList();

    yuiDebug() << "Creating patch list done" << endl;
}


YQPkgPatchList::~YQPkgPatchList()
{
    // NOP
}


void
YQPkgPatchList::polish()
{
    // Delayed initialization after widget is fully created etc.

    // Only now send currentItemChanged() signal so attached details views also
    // display something if their showDetailsIfVisible() slot is connected to
    // currentItemChanged() signals.
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
    clear();
    yuiDebug() << "Filling patch list" << endl;

    for ( ZyppPoolIterator it = zyppPatchesBegin();
	  it != zyppPatchesEnd();
	  ++it )
    {
	ZyppSel	  selectable = *it;
	ZyppPatch zyppPatch = tryCastToZyppPatch( selectable->theObj() );

	if ( zyppPatch )
	{
	    bool displayPatch = false;

	    switch ( _filterCriteria )
	    {
		case RelevantPatches:	// needed + broken + satisfied (but not installed)

		    if ( selectable->hasInstalledObj() ) // installed?
		    {
			if ( selectable->installedPoolItem().status().isIncomplete() ) // patch broken?
			{
			    // The patch is broken: It had been installed, but the user somehow
			    // downgraded individual packages belonging to the patch to older versions.

			    displayPatch = true;

			    yuiWarning() << "Installed patch is broken: "
					 << zyppPatch->name()
					 << " - " << zyppPatch->summary()
					 << endl;
			}
		    }
		    else // not installed
		    {
			if ( selectable->hasCandidateObj() &&
			     selectable->candidatePoolItem().status().isSatisfied() )
			{
			    // This is a pretty exotic case, but still it might happen:
			    //
			    // The patch itelf is not installed, but it is satisfied because the
			    // user updated all the packages belonging to the patch to the versions
			    // the patch requires. All that is missing now is to get the patch meta
			    // data onto the system. So let's display the patch to give the user
			    // a chance to install it (if he so chooses).

			    displayPatch = true;

			    yuiMilestone() << "Patch satisfied, but not installed yet: "
					   << zyppPatch->name()
					   << " - " << zyppPatch->summary()
					   << endl;
			}
		    }

		    if ( selectable->hasCandidateObj() )	// candidate available?
		    {
			// The most common case: There is a candidate patch, i.e. one that could be
			// installed, but either no version of that patch is installed or there is a
			// newer one to which the patch could be updated.

			if ( selectable->candidatePoolItem().status().isNeeded() ) // patch really needed?
			{
			    // Patches are needed if any of the packages that belong to the patch
			    // are installed on the system.

			    displayPatch = true;
			}
			else
			{
			    // None of the packages that belong to the patch is installed on the system.

			    yuiDebug() << "Patch not needed: " << zyppPatch->name()
				       << " - " << zyppPatch->summary()
				       << endl;
			}
		    }
		    break;


		case RelevantAndInstalledPatches:	// needed + broken + installed

		    if ( selectable->hasInstalledObj() ) // installed?
		    {
			displayPatch = true;
		    }
		    else // not installed - display only if needed
		    {
			zypp::ResStatus candidateStatus = selectable->candidatePoolItem().status();

			if ( candidateStatus.isNeeded() ||
			     candidateStatus.isSatisfied() )
			{
			    displayPatch = true;
			}
			else
			{
			    yuiMilestone() << "Patch not needed: " << zyppPatch->name()
					   << " - " << zyppPatch->summary()
					   << endl;
			}
		    }
		    break;


		case AllPatches:
		    displayPatch = true;
		    break;

		// Intentionally omitting "default" so the compiler
		// can catch unhandled enum values
	    }

	    if ( displayPatch )
	    {
#if VERBOSE_PATCH_LIST
		yuiDebug() << "Displaying patch " << zyppPatch->name()
			   << " - " <<  zyppPatch->summary()
			   << endl;
#endif
		addPatchItem( *it, zyppPatch);
	    }
	}
	else
	{
	    yuiError() << "Found non-patch selectable" << endl;
	}
    }

#if FIXME
    if ( ! firstChild() )
	message( _( "No patches available." ) );
#endif

    yuiDebug() << "Patch list filled" << endl;
}



void
YQPkgPatchList::message( const QString & text )
{
    QY2ListViewItem * item = new QY2ListViewItem( this );
    Q_CHECK_PTR( item );

    item->setText( 1, text );
    item->setBackgroundColor( 0, QColor( 0xE0, 0xE0, 0xF8 ) );
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
    std::set<ZyppSel> patchSelectables;

    if ( selection() )
    {
	ZyppPatch patch = selection()->zyppPatch();

	if ( patch )
	{
	    ZyppPatchContents patchContents( patch );

	    for ( ZyppPatchContentsIterator it = patchContents.begin();
		  it != patchContents.end();
		  ++it )
	    {
		ZyppPkg pkg = tryCastToZyppPkg( *it );

		if ( pkg )
		{
		    yuiDebug() << "Found patch pkg: " << (*it)->name()
			       << " arch: " << (*it)->arch().asString()
			       << endl;

		    ZyppSel sel = _selMapper.findZyppSel( pkg );

		    if ( sel )
		    {
			if ( contains( patchSelectables, sel ) )
			{
			    yuiMilestone() << "Suppressing duplicate selectable "
					   << (*it)->name() << "-" << (*it)->edition().asString()
					   << " arch: " << (*it)->arch().asString()
					   << endl;
			}
			else
			{
			    patchSelectables.insert( sel );
			    emit filterMatch( sel, pkg );
			}
		    }
		    else
			yuiError() << "No selectable for pkg " << (*it)->name() << endl;
		}
		else // No ZyppPkg - some other kind of object (script, message)
		{
		    if ( zypp::isKind<zypp::Script> ( *it ) )
		    {
			yuiDebug() << "Found patch script " << (*it)->name() << endl;
			emit filterMatch( _( "Script" ),  fromUTF8( (*it)->name() ), -1 );
		    }
		    else if ( zypp::isKind<zypp::Message> ( *it ) )
		    {
			yuiDebug() << "Found patch message " << (*it)->name() << " (ignoring)" << endl;
		    }
		    else
		    {
			yuiError() << "Found unknown object of kind " << (*it)->kind().asString()
				   << " in patch: " << (*it)->name() << "-" << (*it)->edition().asString()
				   << " arch: " << (*it)->arch().asString()
				   << endl;
		    }
		}
	    }
	}
    }

    emit filterFinished();
}


void
YQPkgPatchList::addPatchItem( ZyppSel	selectable,
			      ZyppPatch zyppPatch )
{
    if ( ! selectable )
    {
	yuiError() << "NULL ZyppSel!" << endl;
	return;
    }

    YQPkgPatchListItem * item = new YQPkgPatchListItem( this, selectable, zyppPatch );
    applyExcludeRules( item );
}


YQPkgPatchListItem *
YQPkgPatchList::selection() const
{
#if FIXME
    QTreeWidgetItem * item = selectedItem();

    if ( ! item )
	return 0;

    return dynamic_cast<YQPkgPatchListItem *> (item);
#else
    return 0;
#endif
}


void
YQPkgPatchList::createNotInstalledContextMenu()
{
    _notInstalledContextMenu = new QMenu( this );
    Q_CHECK_PTR( _notInstalledContextMenu );

    _notInstalledContextMenu->addAction(actionSetCurrentInstall);
    _notInstalledContextMenu->addAction(actionSetCurrentDontInstall);
    _notInstalledContextMenu->addAction(actionSetCurrentTaboo);

    addAllInListSubMenu( _notInstalledContextMenu );
}


void
YQPkgPatchList::createInstalledContextMenu()
{
    _installedContextMenu = new QMenu( this );
    Q_CHECK_PTR( _installedContextMenu );

    _installedContextMenu->addAction(actionSetCurrentKeepInstalled);

#if ENABLE_DELETING_PATCHES
    _installedContextMenu->addAction(actionSetCurrentDelete);
#endif

    _installedContextMenu->addAction(actionSetCurrentUpdate);
    _installedContextMenu->addAction(actionSetCurrentProtected);

    addAllInListSubMenu( _installedContextMenu );
}


QMenu *
YQPkgPatchList::addAllInListSubMenu( QMenu * menu )
{
    QMenu * submenu = new QMenu( menu );
    Q_CHECK_PTR( submenu );

    submenu->addAction(actionSetListInstall);
    submenu->addAction(actionSetListDontInstall);
    submenu->addAction(actionSetListKeepInstalled);

#if ENABLE_DELETING_PATCHES
    submenu->addAction(actionSetListDelete);
#endif

    submenu->addAction(actionSetListUpdate);
    submenu->addAction(actionSetListUpdateForce);
    submenu->addAction(actionSetListTaboo);
    submenu->addAction(actionSetListProtected);

    QAction *action = menu->addMenu(submenu);
    action->setText(_( "&All in This List" ));

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
	    QTreeWidgetItem * selectedListViewItem = currentItem();

	    if ( selectedListViewItem )
	    {
		YQPkgPatchListItem * item = dynamic_cast<YQPkgPatchListItem *> (selectedListViewItem);

		if ( item && item->selectable()->hasInstalledObj() )
		{
		    yuiWarning() << "Deleting patches is not supported" << endl;
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
	case YQPkgYaSTPatch:		setTextColor( 0, QColor( 0, 0, 0xC0 ) );	break;	// medium blue
	case YQPkgSecurityPatch:	setTextColor( 0, Qt::red );		break;
	case YQPkgRecommendedPatch:	setTextColor( 0, QColor( 0, 0, 0xC0 ) );	break;	// medium blue
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
YQPkgPatchListItem::patchCategory( const string & category )
{
    return patchCategory( fromUTF8( category ) );
}


YQPkgPatchCategory
YQPkgPatchListItem::patchCategory( QString category )
{
    category = category.toLower();

    if ( category == "yast"		) return YQPkgYaSTPatch;
    if ( category == "security"		) return YQPkgSecurityPatch;
    if ( category == "recommended"	) return YQPkgRecommendedPatch;
    if ( category == "optional"		) return YQPkgOptionalPatch;
    if ( category == "document"		) return YQPkgDocumentPatch;

    yuiWarning() << "Unknown patch category \"" << category << "\"" << endl;
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

	    text += fromUTF8( zyppPatch()->size().asString().c_str() );
	}
    }

    return text;
}


void
YQPkgPatchListItem::applyChanges()
{
    solveResolvableCollections();
}


bool YQPkgPatchListItem::operator< ( const QTreeWidgetItem & otherListViewItem ) const
{
    const YQPkgPatchListItem * other = dynamic_cast<const YQPkgPatchListItem *> (&otherListViewItem);
    int col = treeWidget()->sortColumn();
    if ( other )
    {
	if ( col == categoryCol() )
	{
	    return ( this->patchCategory() < other->patchCategory() );
	}
    }
    return YQPkgObjListItem::operator<( otherListViewItem );
}



#include "YQPkgPatchList.moc"
