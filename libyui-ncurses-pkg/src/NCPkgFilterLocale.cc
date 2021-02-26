/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:       NCPkgFilterLocale.cc

   Author:     Bubli <kmachalkova@suse.cz> 

/-*/
#define YUILogComponent "ncurses-pkg"
#include <YUILog.h>
#include <sstream>
#include <boost/format.hpp>

#include "NCPkgFilterLocale.h"

/*
  Textdomain "ncurses-pkg"
*/

NCPkgLocaleTag::NCPkgLocaleTag ( zypp::sat::LocaleSupport loc, string status )
    : YTableCell( status )
    , locale ( loc )
{

}

NCPkgLocaleTable::NCPkgLocaleTable( YWidget *parent, YTableHeader *tableHeader, NCPackageSelector *pkg )
    :NCTable( parent, tableHeader )
    ,packager(pkg)
{
   fillHeader();
   fillLocaleList();
}

void NCPkgLocaleTable::fillHeader() 
{
    vector <string> header;

    header.reserve(4);
    header.push_back( "L" + NCPkgStrings::PkgStatus() );
    header.push_back( "L" + NCPkgStrings::LangCode() );
    header.push_back( "L" + NCPkgStrings::LangName() );

    setHeader( header);
}

void NCPkgLocaleTable::addLine ( zypp::sat::LocaleSupport l,  const vector <string> & cols, string status )
{
    //use default ctor, add cell in the next step 
    YTableItem *tabItem = new YTableItem();	

    //place tag (with repo reference) to the 0th column
    tabItem->addCell( new NCPkgLocaleTag ( l, status ) );

    // and append the rest (name, URL and stuff)
    for ( unsigned i = 1; i < cols.size() + 1; ++i ) {
	tabItem->addCell( cols[ i-1 ]);
    }

    // this is NCTable::addItem( tabItem );
    //it actually appends the line to the table
    addItem( tabItem );   

}

string NCPkgLocaleTable::status( zypp::Locale lang )
{
    ZyppStatus status;
    
    if ( zypp::getZYpp()->pool().isRequestedLocale( lang ) )
        status = S_Install;
    else
        status = S_NoInst;

    // convert ZyppStatus to string
    switch ( status )
    {
	case S_NoInst:
	    return "    ";
	case S_Install:
	    return "  i ";
	default:
	    return "####";
    } 
}

void NCPkgLocaleTable::fillLocaleList()
{
    vector <string> oneLine;

    const zypp::LocaleSet & available_locales( zypp::ResPool::instance().getAvailableLocales() );
    for_( it, available_locales.begin(), available_locales.end() )
    {
	oneLine.clear();
        zypp::sat::LocaleSupport myLocale( *it );
        oneLine.push_back( myLocale.locale().code() );
        oneLine.push_back( myLocale.locale().name() ); 
	addLine( myLocale, oneLine, status(*it) );
    }

    myPad()->setOrder(1);
}

NCPkgLocaleTag* NCPkgLocaleTable::getTag (const int & index )
{
   NCTableLine *line = myPad()->ModifyLine( index );
   if ( !line )
   {
	return 0;
   }

   YTableItem *it = dynamic_cast<YTableItem*> (line->origItem() );

   YTableCell *tcell = it->cell(0);
   NCPkgLocaleTag *tag = static_cast<NCPkgLocaleTag *>( tcell );

   return tag;
}

zypp::sat::LocaleSupport NCPkgLocaleTable::getLocale( int index )
{
    NCPkgLocaleTag *t = getTag( index );

	 return t->getLocale();
}

void NCPkgLocaleTable::showLocalePackages()
{
    int index = getCurrentItem();
    zypp::sat::LocaleSupport myLocale = getLocale( index );

    NCPkgTable * packageList = packager->PackageList();
    packageList->itemsCleared();

    yuiMilestone() << "Packages supporting locale '" << myLocale.locale() << "':" << endl;
    for_( it, myLocale.selectableBegin(), myLocale.selectableEnd() )
    {
        ZyppPkg zyppPkg = tryCastToZyppPkg( (*it)->theObj() );
	packageList->createListEntry( zyppPkg, *it );
    }

    ostringstream s;
    //Translators: %s is a locale code, e.g. en_GB
    s << boost::format( _( "Translations, dictionaries and other language related files for <b>%s</b> locale")) % myLocale.locale().code();
    packager->FilterDescription()->setText( s.str() );		

    packageList->setCurrentItem( 0 );
    packageList->drawList();
    packageList->showInformation();
}

void NCPkgLocaleTable::toggleStatus()
{
    int index = getCurrentItem();
    zypp::sat::LocaleSupport myLocale = getLocale( index );
    NCPkgLocaleTag * t = getTag ( index );
    NCTableLine *line = myPad()->ModifyLine( index );

    if ( !t || !line )
	return;
    
    yuiMilestone() << "Toggle status of: " << myLocale.locale().code() << endl;

    if ( zypp::getZYpp()->pool().isRequestedLocale( myLocale.locale() ) )
    {
	zypp::getZYpp()->pool().eraseRequestedLocale( myLocale.locale() );
    }
    else
    {
	zypp::getZYpp()->pool().addRequestedLocale( myLocale.locale() );
    }
    packager->showPackageDependencies( true ); 

    cellChanged( index, 0,  status( myLocale.locale() ) );
}

NCursesEvent NCPkgLocaleTable::wHandleInput( wint_t ch )
{
    NCursesEvent ret = NCursesEvent::none;
    handleInput( ch );

    switch ( ch )
    {
	case KEY_UP:
	case KEY_DOWN:
	case KEY_NPAGE:
	case KEY_PPAGE:
	case KEY_END:
	case KEY_HOME: {
	    ret = NCursesEvent::handled;
            showLocalePackages();
	    break;
	}
	case KEY_SPACE:
	case KEY_RETURN: {
	    ret = NCursesEvent::handled;
	    toggleStatus();
	    showLocalePackages();
	    break;
	}
		
	default:
	    ret = NCTable::wHandleInput( ch ) ;
    }

    return ret;
}

