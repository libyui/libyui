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

  File:	      YQPkgLangList.cc

  Author:     Stefan Hundhammer <sh@suse.de>

  Textdomain "packages-qt"

/-*/


#define y2log_component "qt-pkg"
#include <ycp/y2log.h>
#include <qregexp.h>

#include "YQi18n.h"
#include "utf8.h"
#include "YQPkgLangList.h"

using std::set;


YQPkgLangList::YQPkgLangList( QWidget * parent )
    : YQPkgObjList( parent )
{
    y2debug( "Creating language list" );

    int numCol = 0;
    addColumn( ""		);	_statusCol	= numCol++;

    // Translators: Table column heading for language ISO code like "de_DE", "en_US"
    // Please keep this short to avoid stretching the column too wide!
    addColumn( _( "Code"	) );	_nameCol	= numCol++;

    // Full (human readable) language / country name like "German (Austria)"
    addColumn( _( "Language" ) );	_summaryCol	= numCol++;
    setAllColumnsShowFocus( true );

    connect( this, 	SIGNAL( selectionChanged        ( QListViewItem * ) ),
	     this, 	SLOT  ( filter()                                    ) );

    fillList();
    selectSomething();

    y2debug( "Creating language list done" );
}


YQPkgLangList::~YQPkgLangList()
{
    // NOP
}


void
YQPkgLangList::fillList()
{
    clear();
    y2debug( "Filling language list" );


    for ( ZyppPoolIterator it = zyppLangBegin();
	  it != zyppLangEnd();
	  ++it )
    {
	ZyppLang zyppLang = tryCastToZyppLang( (*it)->theObj() );

	if ( zyppLang )
	    addLangItem( *it, zyppLang );
	else
	    y2error( "Found non-Language selectable" );
    }

    y2debug( "Language list filled" );
}


void
YQPkgLangList::filterIfVisible()
{
    if ( isVisible() )
	filter();
}


void
YQPkgLangList::filter()
{
    emit filterStart();

    if ( selection() )
    {
	ZyppLang lang = selection()->zyppLang();

	if ( lang )
	{
	    string currentLang = lang->name();

	    //
	    // Iterate over all selectables in pool
	    //

	    for ( ZyppPoolIterator it = zyppPkgBegin();
		  it != zyppPkgEnd();
		  ++it )
	    {
		ZyppObj zyppObj = (*it)->theObj();

		if ( zyppObj )
		{
		    //
		    // Iterate over all "freshens" dependencies of this object
		    //

		    zypp::CapSet freshens = zyppObj->dep( zypp::Dep::FRESHENS );

		    for ( zypp::CapSet::const_iterator cap_it = freshens.begin();
			  cap_it != freshens.end();
			  ++cap_it )
		    {
			if ( (*cap_it).index() == currentLang )	// obj freshens this language
			{
			    ZyppPkg pkg = tryCastToZyppPkg( zyppObj );

			    if ( pkg )
			    {
				y2debug( "Found pkg %s for lang %s",
					 pkg->name().c_str(), currentLang.c_str() );

				emit filterMatch( *it, pkg );
			    }
			    else
			    {
				y2warning( "Found non-pkg obj %s for lang %s",
					   pkg->name().c_str(), currentLang.c_str() );
			    }
			}
		    }
		}
	    }
	}
    }

    emit filterFinished();
}


void
YQPkgLangList::addLangItem( ZyppSel	selectable,
			    ZyppLang 	zyppLang )
{
    if ( ! selectable )
    {
	y2error( "NULL ZyppSel!" );
	return;
    }

    new YQPkgLangListItem( this, selectable, zyppLang );
}


YQPkgLangListItem *
YQPkgLangList::selection() const
{
    QListViewItem * item = selectedItem();

    if ( ! item )
	return 0;

    return dynamic_cast<YQPkgLangListItem *> (item);
}






YQPkgLangListItem::YQPkgLangListItem( YQPkgLangList * 	langList,
				      ZyppSel		selectable,
				      ZyppLang 		lang 		)
    : YQPkgObjListItem( langList, selectable, lang )
    , _langList( langList )
    , _zyppLang( lang )
{
}



YQPkgLangListItem::~YQPkgLangListItem()
{
    // NOP
}


void
YQPkgLangListItem::applyChanges()
{
    solveResolvableCollections();
}




#include "YQPkgLangList.moc"
