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
#include "NCSplit.h"
#include "NCSpacing.h"
#include "NCPkgNames.h"
#include "NCPkgTable.h"
#include "NCPkgStatusStrategy.h"
#include <zypp/ui/PatternContents.h>

#include "NCZypp.h"

#ifdef FIXME
#define LOCALE Y2PM::getPreferredLocale()
#else
#define LOCALE
#endif

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
    case S_Pattern:
    case S_Selection: {
        createLayout( YCPString(NCPkgNames::SelectionLabel()) );
	break;
    }
    case S_Language: {
        createLayout( YCPString(NCPkgNames::LanguageLabel()) );
	break;
    }
    case S_Repo: {
	createLayout (YCPString(NCPkgNames::RepoLabel()) );
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
void NCPkgPopupSelection::createLayout( const YCPString & label )
{

  YWidgetOpt opt;

  // the vertical split is the (only) child of the dialog
  NCSplit * split = new NCSplit( this, opt, YD_VERT );
  addChild( split );

  opt.notifyMode.setValue( false );

  //the headline
  opt.isHeading.setValue( true );
  
  NCLabel * head = new NCLabel( split, opt, label );
  split->addChild( head );

  // add the selection list
  sel = new NCPkgTable( split, opt );
  sel->setPackager( packager );
  // set status strategy
  NCPkgStatusStrategy * strat = new SelectionStatStrategy();

  switch (type) {
      case S_Pattern:
      case S_Selection: {
  	  sel->setTableType( NCPkgTable::T_Selections, strat );
	  break;
      }
      case S_Language: {
	  sel->setTableType( NCPkgTable::T_Languages, strat );
	  break;
      }
      case S_Repo: {
	  sel->setTableType( NCPkgTable::T_Repos, strat );
      }
      default: {
	  NCERR << "Unknown selection type" << endl; 	  
	  break;
      }
  }

  sel->fillHeader();
  split->addChild( sel );

  opt.notifyMode.setValue( true );

  //Do not show status help line for repositories
  //it doesn't make sense to add/delete/update those
  if (type != S_Repo)
  {
      NCLabel * help = new NCLabel( split, opt, YCPString(NCPkgNames::DepsHelpLine()) );
      split->addChild( help );
  }

  split->addChild( new NCSpacing( split, opt, 0.4, false, true ) );

  // add an OK button
  opt.key_Fxx.setValue( 10 );
  okButton = new NCPushButton( split, opt, YCPString(NCPkgNames::OKLabel()) );
  okButton->setId( NCPkgNames::OkButton () );

  split->addChild( okButton );

  split->addChild( new NCSpacing( split, opt, 0.4, false, true ) );
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
	    ZyppSelection selPtr = tryCastToZyppSelection (objPtr);
	    ZyppPattern patPtr = tryCastToZyppPattern (objPtr);
	    ZyppLang langPtr = tryCastToZyppLang (objPtr);
	    if (selPtr)
		packages = selPtr->install_packages ();
	    else if (patPtr)
	    {
		zypp::ui::PatternContents patternContents( patPtr );
		packages = patternContents.install_packages();
	    }
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
//	METHOD NAME : NCPkgPopupSelection::niceSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//

long NCPkgPopupSelection::nicesize(YUIDimension dim)
{
    long vdim;
    if ( NCurses::lines() > 20 )
	vdim = 20;
    else
	vdim = NCurses::lines()-4;
	
    return ( dim == YD_HORIZ ? NCurses::cols()*2/3 : vdim );
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

    YCPValue currentId =  dynamic_cast<YWidget *>(postevent.widget)->id();

    if ( !currentId.isNull()
	 && currentId->compare( NCPkgNames::OkButton () ) == YO_EQUAL )
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
// OrderFunc 
//
bool order( ZyppSel slb1, ZyppSel slb2 )
{
    ZyppSelection ptr1 = tryCastToZyppSelection (slb1->theObj());
    ZyppSelection ptr2 = tryCastToZyppSelection (slb2->theObj());
    if ( !ptr1 || !ptr2 )
	return false;
    else
	return ptr1->order() < ptr2->order();
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
    ZyppLang ptr1 = tryCastToZyppLang (slb1->theObj());
    ZyppLang ptr2 = tryCastToZyppLang (slb2->theObj());
    if ( !ptr1 || !ptr2 )
	return false;
    else
	return ptr1->name() < ptr2->name();
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
	case S_Selection:
	    {
		for ( i = zyppSelectionsBegin () ; i != zyppSelectionsEnd ();  ++i )
		{
		    ZyppObj resPtr = (*i)->theObj();	    
		    bool show;

		    ZyppSelection selPtr = tryCastToZyppSelection (resPtr);
		    show = selPtr && selPtr->visible() && !selPtr->isBase();
		    if (show)
		    {
			NCMIL << resPtr->kind () <<": " <<  resPtr->name()
			      << ", initial status: " << (*i)->status() << endl;

			slbList.push_back (*i);
		    }
		}
		slbList.sort( order );

		break;
	    }
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
	    case S_Pattern:
	    case S_Selection: {
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
