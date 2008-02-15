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

   File:       NCPkgPopupSelection.cc

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCPkgPopupSelection.h"

#include "YDialog.h"
#include "NCLayoutBox.h"
#include "NCSpacing.h"
#include "NCPkgNames.h"
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
  Textdomain "packages"
*/

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupSelection::NCPkgPopupSelection
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCPkgPopupSelection::NCPkgPopupSelection( const wpos at,  NCPackageSelector * pkg, SelType type  )
    : NCPopup( at, false )
    , sel( 0 )
    , okButton( 0 )
    , packager( pkg )
    , type( type )
{
  switch ( type ) 
  {   
    case S_Pattern: {
        createLayout( NCPkgNames::SelectionLabel() );
	break;
    }
    case S_Language: {
        createLayout( NCPkgNames::LanguageLabel() );
	break;
    }
    default:
	break;
  }


    fillSelectionList( sel, type );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupSelection::~NCPkgPopupSelection
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCPkgPopupSelection::~NCPkgPopupSelection()
{

}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupSelection::createLayout
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPkgPopupSelection::createLayout( const string & label )
{
  // the vertical split is the (only) child of the dialog
  NCLayoutBox * split = new NCLayoutBox( this, YD_VERT );

  new NCLabel( split, label, true, false );	// isHeading = true
  YTableHeader * tableHeader = new YTableHeader();
  
  // add the selection list
  sel = new NCPkgTable( split, tableHeader );
  sel->setPackager( packager );

  // set status strategy
  NCPkgStatusStrategy * strat = new SelectionStatStrategy();

  switch (type) {
      case S_Pattern: {
  	  sel->setTableType( NCPkgTable::T_Selections, strat );
	  break;
      }
      case S_Language: {
	  sel->setTableType( NCPkgTable::T_Languages, strat );
	  break;
      }
      default: {
	  NCERR << "Unknown selection type" << endl; 	  
	  break;
      }
  }

  sel->fillHeader();

  // a help line for the selction/pattern/language popup
  new NCLabel( split,  _( " [+] Select    [-] Delete    [>] Update " ), false, false );

  new NCSpacing( split, YD_VERT, false, 0.4 );
  
  // add an OK button
  okButton = new NCPushButton( split, NCPkgNames::OKLabel() );
  okButton->setFunctionKey(10);
  
  new NCSpacing( split, YD_VERT, false, 0.4 );
}

///////////////////////////////////////////////////////////////////
//
// NCursesEvent & showSelectionPopup ()
//
//
NCursesEvent & NCPkgPopupSelection::showSelectionPopup( )
{
    postevent = NCursesEvent();

    if ( !sel )
	return postevent;

    sel->updateTable();
    sel->setKeyboardFocus();

    // event loop
    do {
	popupDialog();
    } while ( postAgain() );

    popdownDialog();

    if ( !packager )
	return postevent;

    // if OK is clicked get the current item and show the package list
    if ( postevent.detail == NCursesEvent::USERDEF )
    {
	int index = sel->getCurrentItem();
	ZyppObj objPtr = sel->getDataPointer( index );
	if ( objPtr )
	{
	    NCMIL << "Current selection: " << getCurrentLine() << endl;

	    // show the package list
	    std::set<std::string> packages;
	    ZyppPattern patPtr = tryCastToZyppPattern (objPtr);
	    // FIXME ZyppLang langPtr = tryCastToZyppLang (objPtr);
	    if (patPtr)
	    {
		zypp::ui::PatternContents patternContents( patPtr );
		packages = patternContents.install_packages();
	    }
#warning Languages support
#if FIXME
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

	    packager->showSelPackages( getCurrentLine(), packages );
	    // showDiskSpace() moved to NCPkgTable.cc (show/check diskspace
	    // on every status change)
	    // packager->showDiskSpace();
	}
    }

    return postevent;
}


//////////////////////////////////////////////////////////////////
//
// getCurrentLine()
//
// returns the currently selected list item (may be "really" selected
// or not)
//
string  NCPkgPopupSelection::getCurrentLine( )
{
    if ( !sel )
	return "";

    int index = sel->getCurrentItem();
    ZyppObj selPtr = sel->getDataPointer(index);

    return ( selPtr?selPtr->summary(LOCALE):"" );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupSelection::preferredWidth
//	METHOD TYPE : int
//
int NCPkgPopupSelection::preferredWidth()
{
    return NCurses::cols()*2/3;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupSelection::preferredHeight
//	METHOD TYPE : int
//
int NCPkgPopupSelection::preferredHeight()
{
    if ( NCurses::lines() > 20 )
	return 20;
    else
	return NCurses::lines()-4;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopup::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCPkgPopupSelection::wHandleInput( wint_t ch )
{
    if ( ch == 27 ) // ESC
	return NCursesEvent::cancel;

    return NCDialog::wHandleInput( ch );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupSelection::postAgain
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCPkgPopupSelection::postAgain( )
{
    if( !postevent.widget )
	return false;

    postevent.detail = NCursesEvent::NODETAIL;

    if ( postevent.widget == okButton )
    {
	postevent.detail = NCursesEvent::USERDEF ;
	// return false means: close the popup
	return false;
    }

    if (postevent == NCursesEvent::cancel)
	return false;

    return true;
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
	return ptr1->order() < ptr2->order();
}

///////////////////////////////////////////////////////////////////
//
// OrderFuncLang 
//
bool orderLang( ZyppSel slb1, ZyppSel slb2 )
{
#warning Language support
#if FIXME
    ZyppLang ptr1 = tryCastToZyppLang (slb1->theObj());
    ZyppLang ptr2 = tryCastToZyppLang (slb2->theObj());
    if ( !ptr1 || !ptr2 )
	return false;
    else
	return ptr1->name() < ptr2->name();
#endif
    return false;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupSelection::fillSelectionList
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCPkgPopupSelection::fillSelectionList( NCPkgTable * sel, SelType type  )
{
    if ( !sel )
	return false;

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
			NCMIL << resPtr->kind () <<": " <<  resPtr->name()
			      << ", initial status: " << (*i)->status() << endl;

			slbList.push_back (*i);
		    }
		}
		slbList.sort( orderPattern );
		break;
	    }
#warning Languages support
#if FIXME
        case S_Language:
	    {
		for (i = zyppLangBegin (); i != zyppLangEnd (); ++i )
	        {
		    ZyppObj resPtr = (*i)->theObj();

		    ZyppLang langPtr = tryCastToZyppLang (resPtr);

		    NCMIL << resPtr->kind () <<": " <<  resPtr->name()
		    << ", initial status: " << (*i)->status() << endl;
		    slbList.push_back (*i);

		} 
		slbList.sort( orderLang );
		break;
	    }
#endif
	default:
	    NCERR << "Selecion type not handled: " << type << endl;
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

	sel->addLine( (*listIt)->status(),	// the status
		      pkgLine,
		      resPtr, // ZyppSelection or ZyppPattern
		      (*listIt) ); // ZyppSel
    }
    
    return true;
}
