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

    zypp::LocaleSet locales = zypp::getZYpp()->pool().getAvailableLocales();
    for ( zypp::LocaleSet::const_iterator it = locales.begin();
	  it != locales.end();
	  ++it )
    {
        addLangItem( *it );
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
        zypp::Locale lang = selection()->zyppLang();

        string currentLang = lang.name();

#warning FIXME: need a way to find out what packages provide language support
        // not yet implemented
    }

    emit filterFinished();
}


void
YQPkgLangList::addLangItem( const zypp::Locale &zyppLang )
{
    new YQPkgLangListItem( this, zyppLang );
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
                                      const zypp::Locale &lang )
    : YQPkgObjListItem( langList, 0 )
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
