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

   File:       NCPopupDeps.cc

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include "Y2Log.h"
#include "NCPopupDeps.h"

#include "NCTree.h"
#include "YMenuButton.h"
#include "YDialog.h"
#include "NCSplit.h"
#include "NCSpacing.h"
#include "PkgNames.h"
#include "NCPkgTable.h"
#include "NCMenuButton.h"
#include "NCPushButton.h"
#include "NCPopupInfo.h"
#include "ObjectStatStrategy.h"

#include <y2pm/PMPackageManager.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupDeps::NCPopupDeps
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCPopupDeps::NCPopupDeps( const wpos at, PackageSelector * pkger )
    : NCPopup( at, false )
      , cancelButton( 0 )
      , solveButton( 0 )
      , deps( 0 )
      , head( 0 )
      , errorLabel1( 0 )
      , errorLabel2( 0 )
      , packager( pkger )
      , pkgs( 0 )

{
    createLayout();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupDeps::~NCPopupDeps
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCPopupDeps::~NCPopupDeps()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupSelDeps::createLayout
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPopupDeps::createLayout( )
{

  YWidgetOpt opt;

  // vertical split is the (only) child of the dialog
  NCSplit * vSplit = new NCSplit( this, opt, YD_VERT );
  addChild( vSplit );

  // opt.vWeight.setValue( 40 );

  opt.notifyMode.setValue( true );

  NCSpacing * sp0 = new NCSpacing( vSplit, opt, 0.8, false, true );
  vSplit->addChild( sp0 );

  // add the headline
  opt.isHeading.setValue( true );

  head = new NCLabel( vSplit, opt, YCPString("") );
  vSplit->addChild( head );

  NCSpacing * sp = new NCSpacing( vSplit, opt, 0.4, false, true );
  vSplit->addChild( sp );

  // add the list containing packages with unresolved depemdencies
  pkgs = new NCPkgTable( vSplit, opt );
  pkgs->setPackager( packager );
 
  vSplit->addChild( pkgs );

  NCSpacing * sp1 = new NCSpacing( vSplit, opt, 0.2, false, true );
  vSplit->addChild( sp1 );

  opt.isHStretchable.setValue( true );
  opt.isHeading.setValue( false );
 
  errorLabel1 = new NCLabel(  vSplit, opt, YCPString("") );
  vSplit->addChild( errorLabel1 );
  errorLabel2 = new NCLabel(  vSplit, opt, YCPString("") );
  vSplit->addChild( errorLabel2 );

  NCSpacing * sp2 = new NCSpacing( vSplit, opt, 0.2, false, true );
  vSplit->addChild( sp2 );

  // add the package list containing the dependencies
  deps = new NCPkgTable( vSplit, opt );
  deps->setPackager( packager );
  // set status strategy
  ObjectStatStrategy * strat = new DependencyStatStrategy();
  deps->setTableType( NCPkgTable::T_DepsPackages, strat );
  vSplit->addChild( deps );
  
  opt.isHStretchable.setValue( false );
  opt.isHeading.setValue( true );
  NCLabel * helplb = new NCLabel( vSplit, opt, YCPString(PkgNames::DepsHelpLine().str()) );
  vSplit->addChild( helplb );
  
  NCSpacing * sp3 = new NCSpacing( vSplit, opt, 0.6, false, true );
  vSplit->addChild( sp3 );
  
  NCSplit * hSplit = new NCSplit( vSplit, opt, YD_HORIZ );
  vSplit->addChild( hSplit );

  opt.isHStretchable.setValue( true );

  // add the solve button
  solveButton = new NCPushButton( hSplit, opt, YCPString(PkgNames::SolveLabel().str()) );
  solveButton->setId( PkgNames::Solve () );
  hSplit->addChild( solveButton );

  NCSpacing * sp4 = new NCSpacing( hSplit, opt, 0.4, true, false );
  hSplit->addChild( sp4 );
  
  // add the cancel button
  cancelButton = new NCPushButton( hSplit, opt, YCPString(PkgNames::CancelIgnore().str()) );
  cancelButton->setId( PkgNames::Cancel () );
  hSplit->addChild( cancelButton );

}

///////////////////////////////////////////////////////////////////
//
//  showDependencies
// 
//
void NCPopupDeps::showDependencies( )
{
    // set headline and table type
    head->setLabel( YCPString(getHeadline()) );
    setDepsTableType();
    
    // fill the table header
    pkgs->fillHeader( );
	    
    // 	typedef std::list<Result> ResultList;
    PkgDep::ResultList		goodList;
    
    //	typedef std::list<ErrorResult> ErrorResultList;
    PkgDep::ErrorResultList	badList;
       
    NCDBG << "Solving..." << endl ;

    NCPopupInfo info( wpos(10, 10),  YCPString( "" ), YCPString(PkgNames::Solving().str()), false );
    info.setNiceSize( 18, 4 );
    info.popup();
    
    // call Y2PM::packageManager() or Y2PM::selectionManager() to get the "badlist"
    bool success = solveInstall( goodList, badList );

    info.popdown();

    if ( !success )
    {
	// fill the list with packages which have unresolved deps
	evaluateErrorResult( pkgs, badList );
	
	// show first dependency
	concretelyDependency( pkgs->getCurrentItem() );
	
	showDependencyPopup();    // show the dependencies

	pkgs->setKeyboardFocus();

    }
    
}


bool NCPopupDeps::evaluateErrorResult( NCPkgTable * table,
				       const PkgDep::ErrorResultList & errorlist  )
{
    if ( !table )
	return false;

    // clear the deps vector !!!
    dependencies.clear();
    // clear list
    table->itemsCleared ();
    
    // fill the dependencies vector and create the list of "bad" packages
    list<PkgDep::ErrorResult>::const_iterator it = errorlist.begin();

    
    while ( it != errorlist.end() )
    {
	if ( !(*it).unresolvable.empty() )
	{
	    addDepsLine( table, (*it), PkgNames::RequText().str() ); 
	    dependencies.push_back( make_pair( (*it), PkgNames::RequText().str() ) );
	}

	if ( !(*it).alternatives.empty() )
	{
	    addDepsLine( table, (*it), PkgNames::NeedsText().str() );
	    dependencies.push_back( make_pair( (*it), PkgNames::NeedsText().str() ) );
	}

	if ( !(*it).conflicts_with.empty() )
	{
	    addDepsLine( table, (*it), PkgNames::ConflictText().str() );
	    dependencies.push_back( make_pair( (*it), PkgNames::ConflictText().str() ) );
	}

	if ( !(*it).referers.empty()
	     && (*it).conflicts_with.empty()
	     && (*it).unresolvable.empty()
	     && (*it).alternatives.empty() )
	{
	    addDepsLine( table, (*it), PkgNames::RequByText().str());
	    dependencies.push_back( make_pair( (*it), PkgNames::RequByText().str() ) );
	}
	++it;
    }

    table->drawList();
    
    return true;
}


void NCPopupDeps::addDepsLine( NCPkgTable * table,
			       const PkgDep::ErrorResult & error,
			       string kind )
{
    vector<string> pkgLine;
    pkgLine.reserve(4);

    PMObjectPtr objPtr = error.solvable;
    
    if ( objPtr )
    {
	pkgLine.push_back( objPtr->getSelectable()->name() );	// package name
	pkgLine.push_back( kind );
	if ( kind != PkgNames::RequByText().str()
	     && !error.referers.empty() )
	{
	    pkgLine.push_back( getReferersList( error) );
	}

	table->addLine( objPtr->getSelectable()->status(), //  get the package status
			pkgLine,
			objPtr );	// the corresponding package pointer
	
    }
    else
    {
	pkgLine.push_back( error.name );
	pkgLine.push_back( kind );
	if ( kind != PkgNames::RequByText().str()
	     && !error.referers.empty() )
	{
	    pkgLine.push_back( getReferersList( error) );
	}
	table->addLine( PMSelectable::S_NoInst, // use status not installed
			pkgLine,
			PMObjectPtr() );	// no pointer available 
    }
}

string NCPopupDeps::getReferersList( const PkgDep::ErrorResult & error )
{
    PMObjectPtr lastPtr;
    string lastName = "";
    unsigned int i = 0;
    
    PMObjectPtr objPtr = error.solvable;
    
    list<PkgDep::RelInfo>::const_iterator it = error.referers.begin();
    string refList = "(";

    if ( objPtr )
    {
	refList += objPtr->getSelectable()->name();
    }
    else
    {
	refList += error.name;
    }
    refList += " is required by ";

    while ( it != error.referers.end() )
    {
	PMObjectPtr objPtr = (*it).solvable; 

	if ( objPtr
	     && (lastPtr != objPtr) )
	{
	    refList += objPtr->getSelectable()->name();
	    if ( i < error.referers.size()-1 )
		refList += ", ";
	    lastPtr =  (*it).solvable;
	    lastName = objPtr->getSelectable()->name();
	}
	else if ( (*it).name != lastName )
	{
	    refList += (*it).name;
	    if ( i < error.referers.size()-1 )
	        refList += ", ";
	    lastName = (*it).name; 
	}
	++it;
	i++;
    }
    refList += ")";
	    
    return refList;	    
}

bool NCPopupDeps::concretelyDependency( int index )
{
    unsigned int size = dependencies.size();
    vector<string> pkgLine;
    pkgLine.reserve(4);
    bool labelSet = false;
    
    deps->itemsCleared();

    if ( index < 0 || (unsigned int)index >= size )
	return false;

    // get the ErrorResult
    PkgDep::ErrorResult error = dependencies[index].first;
	
    NCMIL << "*** Showing: " << error << endl;	

    // get the dependencies
    
    if ( dependencies[index].second == PkgNames::RequText().str() )
    {
	list<PkgDep::RelInfo>::iterator it = error.unresolvable.begin();
	while ( it != error.unresolvable.end() )
	{
	    pkgLine.clear();
	    PMObjectPtr objPtr = (*it).solvable;	// not needed here 

	    pkgLine.push_back( (*it).rel.asString() );
	    deps->addLine( PMSelectable::S_NoInst, // use status NOInst
			   pkgLine,
			   PMObjectPtr() );	// null pointer
		
	    ++it;
	}
	    
	errorLabel1->setLabel( YCPString(getLabelRequire1())  );
	errorLabel2->setLabel( YCPString(PkgNames::LabelRequire2().str()) );
    }
    else if ( dependencies[index].second == PkgNames::NeedsText().str() )
    {
	PMObjectPtr lastPtr;

	list<PkgDep::Alternative>::iterator it = error.alternatives.begin();
	while ( it != error.alternatives.end() )
	{
	    pkgLine.clear();
	    PMObjectPtr objPtr = (*it).solvable; 

	    if ( objPtr
		 && ( lastPtr != objPtr ) )
	    {
		pkgLine.push_back( objPtr->getSelectable()->name() );	// package name
		pkgLine.push_back( objPtr->summary() );
	    
		deps->addLine( objPtr->getSelectable()->status(), //  get the package status
			       pkgLine,
			       objPtr );	// the corresponding package
		lastPtr = (*it).solvable;	
	    }
	    ++it;
	}

	errorLabel1->setLabel( YCPString(PkgNames::LabelAlternative().str()) );
	errorLabel2->setLabel( YCPString( "" ) );
    }
    else if ( dependencies[index].second == PkgNames::ConflictText().str() )
    {
	PMObjectPtr lastPtr;
	string causeName = "";
	
	list<PMSolvablePtr>::iterator it = error.remove_to_solve_conflict.begin();

	// show the list of packages which have to be removed
	while ( it != error.remove_to_solve_conflict.end() )
	{
	    pkgLine.clear();
	    PMObjectPtr objPtr = (*it);
	    if ( objPtr )
	    {
		pkgLine.push_back( objPtr->getSelectable()->name() );	// package name
		pkgLine.push_back( objPtr->summary() );
	    
		deps->addLine( objPtr->getSelectable()->status(), //  get the package status
			       pkgLine,
			       objPtr );	// the corresponding package
	    }
	    ++it;
	}
	errorLabel1->setLabel( YCPString(PkgNames::LabelConflict1().str()) );
	errorLabel2->setLabel( YCPString(getLabelConflict2()) );
	labelSet = true;
    }
    else if ( dependencies[index].second == PkgNames::RequByText().str()
	 && error.conflicts_with.empty()
	 && error.unresolvable.empty()
	 && error.alternatives.empty() )
    {
	PMObjectPtr lastPtr;
	string lastName = "";
	
	list<PkgDep::RelInfo>::iterator it = error.referers.begin();

	while ( it != error.referers.end() )
	{
	    pkgLine.clear();
	    PMObjectPtr objPtr = (*it).solvable; 

	    if ( objPtr
		 && (lastPtr != objPtr) )
	    {
		pkgLine.push_back( objPtr->getSelectable()->name() );	// package name
		pkgLine.push_back( objPtr->summary() );
	    
		deps->addLine( objPtr->getSelectable()->status(), //  get the package status
			       pkgLine,
			       objPtr );	// the corresponding package

		lastPtr =  (*it).solvable;
		lastName = objPtr->getSelectable()->name();
	    }
	    else if ( (*it).name != lastName )
	    {
		pkgLine.push_back( (*it).name );
		deps->addLine( PMSelectable::S_NoInst, // use status NOInst
			       pkgLine,
			       PMObjectPtr() );	// null pointer

		lastName = (*it).name; 
	    }
	    ++it;
	}

	PMObjectPtr causePtr = error.solvable;
	if ( causePtr
	     && (causePtr->getSelectable()->status() == PMSelectable::S_Del) )
	{
	    errorLabel1->setLabel( YCPString(getLabelRequBy1()) );
	    errorLabel2->setLabel( YCPString(getLabelRequBy2()) );
	}
	else
	{
	    errorLabel1->setLabel( YCPString(getLabelContinueRequ()) );
	    errorLabel2->setLabel( YCPString( "" ) );
	}
    }

    // show the list
    deps->drawList();
    
    return true;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupDeps::showDependencyPopup
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
NCursesEvent NCPopupDeps::showDependencyPopup( )
{
    postevent = NCursesEvent();

    do {
	popupDialog();
    } while ( postAgain() );

    popdownDialog();

    return postevent;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupDeps::niceSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//

long NCPopupDeps::nicesize(YUIDimension dim)
{
    return ( dim == YD_HORIZ ? NCurses::cols()-15 : NCurses::lines()-5 );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopup::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCPopupDeps::wHandleInput( int ch )
{
    if ( ch == 27 ) // ESC
	return NCursesEvent::cancel;

    return NCDialog::wHandleInput( ch );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupDeps::postAgain
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCPopupDeps::postAgain()
{
    if ( ! postevent.widget )
	return false;

    YCPValue currentId =  dynamic_cast<YWidget *>(postevent.widget)->id();

    if ( currentId->compare( PkgNames::Cancel () ) == YO_EQUAL )
    {
	return false;
    }
    else if  ( currentId->compare( PkgNames::Solve () ) == YO_EQUAL )
    {
	// get currently selected package
	PMObjectPtr currentPtr = pkgs->getDataPointer( pkgs->getCurrentItem() );

	// solve the dependencies and create new list of "bad" packages
	PkgDep::ErrorResultList	badList;
	PkgDep::ResultList	goodList;
   
	NCDBG << "Solving..." << endl ;

	// call Y2PM::packageManager() or Y2PM::selectionManager() to get the "badlist"
	bool success = solveInstall( goodList, badList );

	if ( !success )
	{
	    // fill the list with packages  which have unresolved deps
	    evaluateErrorResult( pkgs, badList );

	    // set current item ( if the package is still there )
	    if ( currentPtr )
	    {
		unsigned int size = pkgs->getNumLines();
		unsigned int index = 0;
		PMObjectPtr pkgPtr;
		while ( index < size )
		{
		    pkgPtr = pkgs->getDataPointer( index );
		    if ( pkgPtr == currentPtr )
		    {
			NCDBG << "Setting current package line: " << index << endl;
			pkgs->setCurrentItem( index );
			break;
		    }
		    index ++;
		}
	    }
	    pkgs->setKeyboardFocus();

	    concretelyDependency( pkgs->getCurrentItem() );
	}
	else	// everything ok
	{
	    pkgs->itemsCleared();
	    deps->itemsCleared();
	    vector<string> line;
	    line.reserve(2);
	    // show a line with text: no conflicts ...
	    line.push_back( PkgNames::NoConflictText().str() );
	    pkgs->addLine( PMSelectable::S_NoInst,
			   line,
			   PMObjectPtr() );
	    pkgs->drawList();

            // close the dialog automatically
	    postevent = NCursesEvent::cancel;
	}
    }

    if ( postevent == NCursesEvent::cancel )
    {
	// return false means: close the popup dialog
	return false;
    }
    return true;
}
