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

   File:       NCPkgFilterPattern.cc

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#define YUILogComponent "ncurses-pkg"
#include <YUILog.h>

#include "NCPkgFilterPattern.h"

#include "YDialog.h"
#include "NCLayoutBox.h"
#include "NCSpacing.h"
#include "NCPkgStrings.h"
#include "NCPkgTable.h"
#include "NCPkgStatusStrategy.h"
#include <zypp/ui/PatternContents.h>
#include <zypp/sat/LocaleSupport.h>

#include "NCZypp.h"
#include "NCi18n.h"

#ifdef FIXME
#define LOCALE Y2PM::getPreferredLocale()
#else
#define LOCALE
#endif

/*
  Textdomain "ncurses-pkg"
*/

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgFilterPattern::NCPkgFilterPattern
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCPkgFilterPattern::NCPkgFilterPattern( YWidget *parent, YTableHeader *header, NCPackageSelector * pkg )
    : NCPkgTable( parent, header )
    , packager( pkg )
{
   createLayout( parent );
   setNotify(true);
   fillPatternList(  );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgFilterPattern::~NCPkgFilterPattern
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCPkgFilterPattern::~NCPkgFilterPattern()
{

}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgFilterPattern::createLayout
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPkgFilterPattern::createLayout( YWidget *parent )
{

    setPackager( packager );
    
    // set status strategy
    NCPkgStatusStrategy * strat = new SelectionStatStrategy();
    
    setTableType( NCPkgTable::T_Selections, strat );

    fillHeader();

}

///////////////////////////////////////////////////////////////////
//
// NCursesEvent & showSelectionPopup ()
//
//
void NCPkgFilterPattern::showPatternPackages( )
{
    int index = getCurrentItem();
    ZyppObj objPtr = getDataPointer( index );
    if ( objPtr )
    {
        // show the package list
        std::set<std::string> packages;
        ZyppPattern patPtr = tryCastToZyppPattern (objPtr);
        
        if (patPtr)
        {
            int total = 0;
            int installed = 0;
        	
            yuiMilestone() << "Show packages belonging to selected pattern: " << getCurrentLine() << endl; 
            NCPkgTable * packageList = packager->PackageList();
        
            if ( !packageList )
            {
        	   yuiError() << "Widget is not a valid NCPkgTable widget" << endl;
        	   return;
            }
            packageList->itemsCleared ();
        
            zypp::Pattern::Contents related ( patPtr->contents() );
            for ( zypp::Pattern::Contents::Selectable_iterator it = related.selectableBegin();
        	  it != related.selectableEnd();
        	  ++it )
            {
        	    ZyppPkg zyppPkg = tryCastToZyppPkg( (*it)->theObj() );
        	    if ( zyppPkg )
        	    {
        	        packageList->createListEntry( zyppPkg, *it );
        	        if ( (*it)->installedSize() > 0 )
        	    	++installed;
        	        ++total;
        	    			
        	    }
            }
            packager->FilterDescription()->setText ( showDescription( objPtr ) );
            char buf[100];
            sprintf(buf, "%d of %d packages installed", installed, total);
        
            packager->PatternLabel()->setLabel ( buf );
        
            packageList->setCurrentItem( 0 );
            packageList->drawList();
            packageList->showInformation();
        }
    }    
}


//////////////////////////////////////////////////////////////////
//
// getCurrentLine()
//
// returns the currently selected list item (may be "really" selected
// or not)
//
string  NCPkgFilterPattern::getCurrentLine( )
{
//if ( !sel )
//	return "";

    int index = getCurrentItem();
    ZyppObj selPtr = getDataPointer(index);
    
    return ( selPtr?selPtr->summary(LOCALE):"" );
}

string NCPkgFilterPattern::showDescription( ZyppObj objPtr )
{
    ZyppPattern patPtr = tryCastToZyppPattern (objPtr);
    return patPtr->description(); 
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopup::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCPkgFilterPattern::wHandleInput( wint_t ch )
{
    NCursesEvent ret = NCursesEvent::none;
    
    // call handleInput of NCPad
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
        break;
    }
    
    default:
       ret = NCPkgTable::wHandleInput( ch ) ;
    }
    
    showPatternPackages( );
    return ret;
}

///////////////////////////////////////////////////////////////////
//
// OrderFuncPattern 
//
bool orderPattern( ZyppSel slb1, ZyppSel slb2 )
{
    ZyppPattern ptr1 = tryCastToZyppPattern (slb1->theObj());
    ZyppPattern ptr2 = tryCastToZyppPattern (slb2->theObj());
    if ( !ptr1 || !ptr2 )
        return false;
    else
    {
        return ptr1->order() < ptr2->order();
    }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgFilterPattern::fillContainerList
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCPkgFilterPattern::fillPatternList(  )
{

    ZyppPoolIterator i, b, e;
    list<ZyppSel> slbList;
    
    for ( i = zyppPatternsBegin () ; i != zyppPatternsEnd ();  ++i )
    {
        ZyppObj resPtr = (*i)->theObj();	    
        bool show;
    
        ZyppPattern patPtr = tryCastToZyppPattern (resPtr);
        show = patPtr && patPtr->userVisible ();
    
        if (show)
        {
    	    yuiMilestone() << resPtr->kind () <<": " <<  resPtr->name()
    	      << ", initial status: " << (*i)->status() << endl;
    	    slbList.push_back (*i);
        }
    }
    slbList.sort( orderPattern );
    
    list<ZyppSel>::iterator listIt;
    vector<string> pkgLine;
    for ( listIt = slbList.begin(); listIt != slbList.end(); ++listIt )
    {
	ZyppObj resPtr = (*listIt)->theObj();
	pkgLine.clear();

	pkgLine.push_back( resPtr->summary(LOCALE) ); // the description
	   
	addLine( (*listIt)->status(),	// the status
		      pkgLine,
		      resPtr, // ZyppPattern
		      (*listIt) ); // ZyppSel
    }
    
    return true;
}
