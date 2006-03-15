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
    setAutoApplyChanges( true );

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
#ifdef FIXME_missing_in_zypp
#else
	    y2warning( "Missing: Get packages for a zypp::Language" );
#endif
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



#include "YQPkgLangList.moc"
