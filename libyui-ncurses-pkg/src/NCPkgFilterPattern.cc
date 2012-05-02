/****************************************************************************
|
| Copyright (c) [2002-2011] Novell, Inc.
| All Rights Reserved.
|
| This program is free software; you can redistribute it and/or
| modify it under the terms of version 2 of the GNU General Public License as
| published by the Free Software Foundation.
|
| This program is distributed in the hope that it will be useful,
| but WITHOUT ANY WARRANTY; without even the implied warranty of
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
| GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License
| along with this program; if not, contact Novell, Inc.
|
| To contact Novell about this file by physical or electronic mail,
| you may find current contact information at www.novell.com
|
|***************************************************************************/


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

/-*/
#define YUILogComponent "ncurses-pkg"
#include <YUILog.h>
#include <sstream>
#include <boost/format.hpp>

#include "NCPkgFilterPattern.h"

#include "YDialog.h"
#include "NCLayoutBox.h"
#include "NCSpacing.h"
#include "NCPkgStrings.h"
#include "NCPkgTable.h"
#include "NCPkgStatusStrategy.h"
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

struct paircmp
{
    bool operator() (pair<string, string> p1, pair<string, string> p2)
    {
	if( p1.second != p2.second )
            return p1.second < p2.second;
	else
            return ( p1.first < p2.first );
    }    
};
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
                        {
                            ++installed;
                        }
        	        ++total;
        	    }
            }
            packager->FilterDescription()->setText ( showDescription( objPtr ) );

	    ostringstream s;

            s << boost::format( _( "%d of %d package installed", "%d of %d packages installed", total )) % installed % total;
        
            packager->PatternLabel()->setLabel ( s.str() );
        
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
        if( ptr1->order() != ptr2->order() )
            return ( ptr1->order() < ptr2->order() );
	else
            return ( ptr1->name() < ptr2->name() );
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
    map<string, list<ZyppSel> > patterns;
    map<string, list<ZyppSel> >::iterator mapIt;
    
    for ( i = zyppPatternsBegin () ; i != zyppPatternsEnd ();  ++i )
    {
        ZyppObj resPtr = (*i)->theObj();	    
        bool show;
    
        ZyppPattern patPtr = tryCastToZyppPattern (resPtr);
        show = patPtr && patPtr->userVisible ();
    
        if (show)
        {
	    string cat = patPtr->category();

	    //fallback category
	    if ( cat.empty())
		cat = "other";

	    //create "category_name" : list <patterns> pair	    
	    map <string, list<ZyppSel> >::iterator item = patterns.find(cat);
	    if( item == patterns.end())
	    {
		list <ZyppSel> slbList;
		slbList.push_back( (*i) );
		yuiMilestone() << "New category added: " << cat << endl;
		patterns.insert( make_pair (cat,slbList) );
	    }
	    else
	    {
	        (*item).second.push_back( (*i));
	    }

    	    yuiMilestone() << resPtr->kind () <<": " <<  resPtr->name()
    	      << ", initial status: " << (*i)->status() << ", order: " << patPtr->order() << endl;
        }
    }

    set < pair <string, string>, paircmp > pat_index;
    set < pair <string, string>, paircmp >::iterator indexIt;

    //for each category
    for ( mapIt = patterns.begin(); mapIt != patterns.end(); ++mapIt )
    {
        string name = (*mapIt).first;
	//sort the patterns by their order #
        (*mapIt).second.sort( orderPattern );
	list<ZyppSel>::iterator it = (*mapIt).second.begin();

        ZyppPattern pat = tryCastToZyppPattern ((*it)->theObj());

	if (pat)
	{
           yuiDebug() << "Lowest #: "<< pat->order() << endl;
	   //create "category name" : "order #" pair in index structure
           pat_index.insert( make_pair( name, pat->order()) );

	}
    }

    list<ZyppSel>::iterator listIt;
    vector<string> pkgLine;

    //now retrieve patterns in defined order
    for( indexIt = pat_index.begin(); indexIt != pat_index.end(); ++indexIt)
    {
	string name = (*indexIt).first;
	list<ZyppSel> slbList = patterns[name];

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
    }
    
    return true;
}
