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

   File:       NCPkgFilterContainer.cc

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#define YUILogComponent "ncurses-pkg"
#include <YUILog.h>

#include "NCPkgFilterContainer.h"

#include "YDialog.h"
#include "NCLayoutBox.h"
#include "NCSpacing.h"
#include "NCPkgStrings.h"
#include "NCPkgTable.h"
#include "NCPkgStatusStrategy.h"
#include <zypp/ui/PatternContents.h>

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
//	METHOD NAME : NCPkgFilterContainer::NCPkgFilterContainer
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCPkgFilterContainer::NCPkgFilterContainer( YWidget *parent, YTableHeader *header, NCPackageSelector * pkg, SelType type  )
    : NCPkgTable( parent, header )
    , packager( pkg )
    , type( type )
{
  switch ( type ) 
  {   
    case S_Pattern: {
        createLayout( parent, NCPkgStrings::LanguageLabel() );
	break;
    }
    case S_Language: {
        createLayout( parent, NCPkgStrings::LanguageLabel() );
	break;
    }
    default:
	break;
  }

   setNotify(true);
   fillContainerList( type );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgFilterContainer::~NCPkgFilterContainer
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCPkgFilterContainer::~NCPkgFilterContainer()
{

}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgFilterContainer::createLayout
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPkgFilterContainer::createLayout( YWidget *parent, const string & label )
{
  YTableHeader * tableHeader = new YTableHeader();
  
  setPackager( packager );

  // set status strategy
  NCPkgStatusStrategy * strat = new SelectionStatStrategy();

  switch (type) {
      case S_Pattern: {
  	  setTableType( NCPkgTable::T_Selections, strat );
	  break;
      }
      case S_Language: {
	  setTableType( NCPkgTable::T_Languages, strat );
	  break;
      }
      default: {
	  yuiError() << "Unknown selection type" << endl; 	  
	  break;
      }
  }

  fillHeader();

}

///////////////////////////////////////////////////////////////////
//
// NCursesEvent & showSelectionPopup ()
//
//
void NCPkgFilterContainer::showContainerPackages( )
{
    int index = getCurrentItem();
    ZyppObj objPtr = getDataPointer( index );
    if ( objPtr )
    {
        // show the package list
        std::set<std::string> packages;
        ZyppPattern patPtr = tryCastToZyppPattern (objPtr);
        //ZyppLang langPtr = tryCastToZyppLang (objPtr);
        if (patPtr)
        {
	    packager->FilterDescription()->setText ( showDescription( objPtr ) );

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
		}
	    }
    	    packageList->setCurrentItem( 0 );
    	    packageList->drawList();
    	    packageList->showInformation();
        }
 #if 0 //FIXME	
        else if (langPtr)
        {
	    string currentLang = langPtr->name();
    
	    for ( ZyppPoolIterator it = zyppPkgBegin(); it != zyppPkgEnd(); ++it )
            {
                ZyppObj zyppObj = (*it)->theObj();
    
                if ( zyppObj )
                {
		    //find all 'freshens' dependencies of this object
		    zypp::CapSet freshens = zyppObj->dep( zypp::Dep::FRESHENS );
    
		    for ( zypp::CapSet::const_iterator cap_it = freshens.begin();
			  cap_it != freshens.end();
			  ++cap_it )
                    {
                        if ( (*cap_it).index() == currentLang ) // obj freshens this language
                        {
                            ZyppPkg pkg = tryCastToZyppPkg( zyppObj );
    
                            if ( pkg )
                            {
                                NCDBG <<  "Found pkg " << pkg->name().c_str() << "for lang "
				      << currentLang.c_str() << endl;
    
				packages.insert( pkg->name() );
                            }
                        }
                    }
                }
            }
    
        }
#endif
    }
}


//////////////////////////////////////////////////////////////////
//
// getCurrentLine()
//
// returns the currently selected list item (may be "really" selected
// or not)
//
string  NCPkgFilterContainer::getCurrentLine( )
{
    //if ( !sel )
//	return "";

    int index = getCurrentItem();
    ZyppObj selPtr = getDataPointer(index);

    return ( selPtr?selPtr->summary(LOCALE):"" );
}

string NCPkgFilterContainer::showDescription( ZyppObj objPtr )
{
   switch (type)
   {
	case S_Pattern:
	{
	    ZyppPattern patPtr = tryCastToZyppPattern (objPtr);
	    return patPtr->description();
	}
	#if 0
        case S_Language:
	{
	    ZyppLanguage langPtr = tryCastToZyppLang (objPtr);
	    return ( NCPkgStrings::LanguageDescription() + langPtr->name() );
	}
	#endif
	default:
	    break;
   }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopup::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCPkgFilterContainer::wHandleInput( wint_t ch )
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
     
     showContainerPackages( );
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
// OrderFuncLang 
//
#if 0
bool orderLang( ZyppSel slb1, ZyppSel slb2 )
{
    ZyppLang ptr1 = tryCastToZyppLang (slb1->theObj());
    ZyppLang ptr2 = tryCastToZyppLang (slb2->theObj());
    if ( !ptr1 || !ptr2 )
	return false;
    else
	return ptr1->name() < ptr2->name();
}
#endif
///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgFilterContainer::fillContainerList
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCPkgFilterContainer::fillContainerList(  SelType type  )
{
//    if ( !sel )
//	return false;

    ZyppPoolIterator i, b, e;
    list<ZyppSel> slbList;
    
    switch ( type )
    {
	case S_Pattern:
	    {
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
		break;
	    }
        #if 0
        case S_Language:
	    {
		for (i = zyppLangBegin (); i != zyppLangEnd (); ++i )
	        {
		    ZyppObj resPtr = (*i)->theObj();

		    ZyppLang langPtr = tryCastToZyppLang (resPtr);

		    yuiMilestone() << resPtr->kind () <<": " <<  resPtr->name()
		    << ", initial status: " << (*i)->status() << endl;
		    slbList.push_back (*i);

		} 
		slbList.sort( orderLang );
		break;
	    }
        #endif
	default:
	    yuiError() << "Container type not handled: " << type << endl;
    }
    
    list<ZyppSel>::iterator listIt;
    vector<string> pkgLine;
    for ( listIt = slbList.begin(); listIt != slbList.end(); ++listIt )
    {
	ZyppObj resPtr = (*listIt)->theObj();
	pkgLine.clear();

	switch (type) {
	    case S_Pattern: {
		pkgLine.push_back( resPtr->summary(LOCALE) ); // the description
		break;	
	    }
	    case S_Language: {
		pkgLine.push_back( resPtr->name() );
		pkgLine.push_back( resPtr->summary(LOCALE) );
		break;
	    }
	    default:
		break;
	}

	addLine( (*listIt)->status(),	// the status
		      pkgLine,
		      resPtr, // ZyppPattern
		      (*listIt) ); // ZyppSel
    }
    
    return true;
}
