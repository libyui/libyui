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


#define YUILogComponent "qt-pkg"
#include "YUILog.h"
#include <qregexp.h>

#include "YQi18n.h"
#include "utf8.h"
#include "YQPkgLangList.h"

using std::set;


YQPkgLangList::YQPkgLangList( QWidget * parent )
    : YQPkgObjList( parent )
{
    yuiDebug() << "Creating language list" << endl;

    int numCol = 0;
    QStringList headers;
    headers <<  "";	_statusCol	= numCol++;

    // Translators: Table column heading for language ISO code like "de_DE", "en_US"
    // Please keep this short to avoid stretching the column too wide!
    headers <<  _( "Code"	);	_nameCol	= numCol++;

    // Full (human readable) language / country name like "German (Austria)"
    headers <<  _( "Language");	_summaryCol	= numCol++;
    setAllColumnsShowFocus( true );
    setHeaderLabels(headers);

    connect( this, 	SIGNAL( currentItemChanged        ( QTreeWidgetItem *, QTreeWidgetItem * ) ),
	     this, 	SLOT  ( filter()                                    ) );

    fillList();
    selectSomething();

    yuiDebug() << "Creating language list done" << endl;
}


YQPkgLangList::~YQPkgLangList()
{
    // NOP
}


void
YQPkgLangList::fillList()
{
    clear();
    yuiDebug() << "Filling language list" << endl;


    for ( ZyppPoolIterator it = zyppLangBegin();
	  it != zyppLangEnd();
	  ++it )
    {
	ZyppLang zyppLang = tryCastToZyppLang( (*it)->theObj() );

	if ( zyppLang )
	    addLangItem( *it, zyppLang );
	else
	    yuiError() << "Found non-Language selectable" << endl;
    }

    yuiDebug() << "Language list filled" << endl;
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
				yuiDebug() << "Found pkg " << pkg->name()
					   << " for lang " << currentLang
					   << endl;

				emit filterMatch( *it, pkg );
			    }
			    else
			    {
				yuiWarning() << "Found non-pkg obj " << pkg->name()
					     << " for lang "         << currentLang
					     << endl;
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
	yuiError() << "NULL ZyppSel!" << endl;
	return;
    }

    new YQPkgLangListItem( this, selectable, zyppLang );
}


YQPkgLangListItem *
YQPkgLangList::selection() const
{
    QTreeWidgetItem * item = currentItem();

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
