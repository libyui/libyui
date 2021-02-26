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

  Textdomain "qt-pkg"

/-*/


#define y2log_component "qt-pkg"
#include "ycp/y2log.h"
#include <qregexp.h>

//#include <QHeaderView>
#include "YQi18n.h"
#include "utf8.h"
#include "YQPkgLangList.h"

#include "zypp/sat/LocaleSupport.h"

using std::set;


YQPkgLangList::YQPkgLangList( QWidget * parent )
    : YQPkgObjList( parent )
#warning "base class works with zypp::Resolvable, but zypp::Locale isn't one any longer!"
{
    y2debug("Creating language list");

    int numCol = 0;
    addColumn(""); _statusCol = numCol++;
    // Translators: Table column heading for language ISO code like "de_DE", "en_US"
    // Please keep this short to avoid stretching the column too wide!
    addColumn(_("Code")); _nameCol = numCol++;
    addColumn(_("Language")); _summaryCol = numCol++;

    setAllColumnsShowFocus( true );

    connect( this, 	SIGNAL( selectionChanged        ( QListViewItem * ) ),
	     this, 	SLOT  ( filter()                                    ) );

    fillList();
    selectSomething();
    y2debug("Creating language list done");
}


YQPkgLangList::~YQPkgLangList()
{
    // NOP
}


void
YQPkgLangList::fillList()
{
    clear();
    y2debug("Filling language list");

    zypp::LocaleSet locales = zypp::getZYpp()->pool().getAvailableLocales();

    for ( zypp::LocaleSet::const_iterator it = locales.begin();
	  it != locales.end();
	  ++it )
    {
        addLangItem( *it );
    }

    y2debug("Language list filled");
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
        int total = 0;
        int installed = 0;

        zypp::Locale lang = selection()->zyppLang();

        zypp::sat::LocaleSupport myLocale( lang );
        for_( it, myLocale.selectableBegin(), myLocale.selectableEnd() )
        {
            ZyppPkg zyppPkg = tryCastToZyppPkg( (*it)->theObj() );
            if ( zyppPkg )
            {
                if ( (*it)->installedSize() > 0 )
                    ++installed;
                ++total;

                emit filterMatch( *it, zyppPkg );
            }
        }
        //selection()->setInstalledPackages(installed);
        //selection()->setTotalPackages(total);

        //selection()->setText( _summaryCol, QString().sprintf("%s (%d/%d)", zyppPattern->summary().c_str(), installed, total));

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
    QListViewItem * item = selectedItem();

    if ( ! item )
	return 0;

    return dynamic_cast<YQPkgLangListItem *> (item);
}

void
YQPkgLangList::updateActions( YQPkgObjListItem * item)
{
    actionSetCurrentInstall->setEnabled( true );
    actionSetCurrentDontInstall->setEnabled( true );
    actionSetCurrentTaboo->setEnabled( true );
    actionSetCurrentProtected->setEnabled( false );

    actionSetCurrentKeepInstalled->setEnabled( false );
    actionSetCurrentDelete->setEnabled( false );
    actionSetCurrentUpdate->setEnabled( false );
}

YQPkgLangListItem::YQPkgLangListItem( YQPkgLangList * 	   langList,
                                      const zypp::Locale & lang )
    : YQPkgObjListItem( langList )
    , _zyppLang( lang )
{
    init();
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


void
YQPkgLangListItem::init()
{
#warning this is utterly broken - see bug #370233
    // DO NOT CALL PARENT CLASS
    _debugIsBroken	= false;
    _debugIsSatisfied	= false;
    _candidateIsNewer	= false;
    _installedIsNewer 	= false;

    if ( nameCol()    >= 0 )	setText( nameCol(),	_zyppLang.code()	);
    if ( summaryCol() >= 0 )	setText( summaryCol(),	_zyppLang.name()	);

    setStatusIcon();
}


ZyppStatus
YQPkgLangListItem::status() const
{
    if ( zypp::getZYpp()->pool().isRequestedLocale( _zyppLang ) )
        return S_Install;
    else
        return S_NoInst;
}

void
YQPkgLangListItem::setStatus( ZyppStatus newStatus, bool sendSignals )
{
    ZyppStatus oldStatus = status();

    switch ( newStatus )
    {
        case S_Install:
            if ( ! zypp::getZYpp()->pool().isRequestedLocale( _zyppLang ) )
            {
                zypp::getZYpp()->pool().addRequestedLocale( _zyppLang );
            }
            break;
        case S_NoInst:
            if ( zypp::getZYpp()->pool().isRequestedLocale( _zyppLang ) )
            {
                zypp::getZYpp()->pool().eraseRequestedLocale( _zyppLang );
            }
            break;
        default:
            return;
    }

    if ( oldStatus != newStatus )
    {
	applyChanges();

	if ( sendSignals )
	{
	    _pkgObjList->updateItemStates();
	    _pkgObjList->sendUpdatePackages();
	}
    }

    setStatusIcon();
    _pkgObjList->sendStatusChanged();
}

bool
YQPkgLangListItem::bySelection() const
{
    return zypp::getZYpp()->pool().isRequestedLocale( _zyppLang );
}


void
YQPkgLangListItem::cycleStatus()
{
    if ( zypp::getZYpp()->pool().isRequestedLocale( _zyppLang ) )
    {
        zypp::getZYpp()->pool().eraseRequestedLocale( _zyppLang );
    }
    else
    {
        zypp::getZYpp()->pool().addRequestedLocale( _zyppLang );
    }
    setStatusIcon();
    _pkgObjList->sendStatusChanged();
}

#include "YQPkgLangList.moc"
