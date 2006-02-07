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
#include "YQZypp.h"
#include <zypp/ui/ResPoolProxy.h>
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

    PMManager::SelectableVec::const_iterator it = Y2PM::languageManager().begin();

    while ( it != Y2PM::languageManager().end() )
    {
	PMLanguagePtr lang = ( *it)->theObj();

	if ( lang )
	    addLangItem( lang );

	++it;
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
	PMLanguagePtr lang = selection()->pmLang();

	if ( lang )
	{
	    PMLanguageManager::PkgSelectables selectables =
		Y2PM::languageManager().getLangPackagesFor( lang );

	    PMLanguageManager::PkgSelectables::const_iterator it = selectables.begin();
	    while ( it != selectables.end() )
	    {
		emit filterMatch( *it, (*it)->theObj() );
		++it;
	    }
	}
    }

    emit filterFinished();
}


void
YQPkgLangList::addLangItem( zypp::ui::Selectable::Ptr	selectable,
			    PMLanguagePtr 		pmLang )
{
    if ( ! selectable )
    {
	y2error( "NULL zypp::ui::Selectable!" );
	return;
    }

    new YQPkgLangListItem( this, selectable, pmLang );
}


YQPkgLangListItem *
YQPkgLangList::selection() const
{
    QListViewItem * item = selectedItem();

    if ( ! item )
	return 0;

    return dynamic_cast<YQPkgLangListItem *> (item);
}






YQPkgLangListItem::YQPkgLangListItem( YQPkgLangList * 		langList,
				      zypp::ui::Selectable::Ptr	selectable,
				      PMLanguagePtr 		lang )
    : YQPkgObjListItem( langList, selectable, lang )
    , _langList( langList )
    , _pmLang( lang )
{
}



YQPkgLangListItem::~YQPkgLangListItem()
{
    // NOP
}


void
YQPkgLangListItem::setStatus( zypp::ui::Status newStatus )
{
    YQPkgObjListItem::setStatus( newStatus );
    _langList->sendUpdatePackages();
}


#include "YQPkgLangList.moc"
