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
#include "ObjectStatStrategy.h"

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
      , okButton( 0 )
      , solveButton( 0 )
      , pkgs( 0 )
      , deps( 0 )
      , depsMenu( 0 )
      , packager( pkger )
{
    createLayout( PkgNames::PackageDeps() );
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
//	METHOD NAME : NCPopupDeps::createLayout
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCPopupDeps::createLayout( const YCPString & headline )
{

  YWidgetOpt opt;

  // vertical split is the (only) child of the dialog
  NCSplit * vSplit = new NCSplit( this, opt, YD_VERT );
  addChild( vSplit );

  // opt.vWeight.setValue( 40 );

  opt.notifyMode.setValue( true );

  NCSpacing * sp0 = new NCSpacing( vSplit, opt, 0.2, false, true );
  vSplit->addChild( sp0 );

  // add the headline
  opt.isHeading.setValue( true );
  NCLabel * head = new NCLabel( vSplit, opt, headline );
  vSplit->addChild( head );

  NCSpacing * sp = new NCSpacing( vSplit, opt, 0.8, false, true );
  vSplit->addChild( sp );

  // add the list containing packages with unresolved depemdencies
  pkgs = new NCPkgTable( vSplit, opt );
  pkgs->setPackager( packager );
  // set status strategy
  ObjectStatStrategy * strategy =
      new DependencyStatStrategy();
  pkgs->setStatusStrategy( strategy );
  vSplit->addChild( pkgs );

  depsMenu = new NCMenuButton( vSplit, opt, YCPString( "Dependencies" ) );
  depsMenu->addMenuItem( PkgNames::RequiredBy(), PkgNames::RequRel() );
  depsMenu->addMenuItem( PkgNames::ConflictDeps(), PkgNames::ConflRel() );
  depsMenu->addMenuItem( PkgNames::Alternatives(), PkgNames::AlterRel() );

  vSplit->addChild( depsMenu );

  NCSpacing * sp1 = new NCSpacing( vSplit, opt, 0.8, false, true );
  vSplit->addChild( sp1 );

  // add the package list containing the dependencies
  deps = new NCPkgTable( vSplit, opt );
  deps->setPackager( packager );
  // set status strategy
  strategy = new AvailableStatStrategy();
  deps->setStatusStrategy( strategy );
  vSplit->addChild( deps );

  NCSplit * hSplit = new NCSplit( vSplit, opt, YD_HORIZ );
  vSplit->addChild( hSplit );

  // add the cancel button
  opt.isHStretchable.setValue( false );
  cancelButton = new NCPushButton( hSplit, opt, PkgNames::CancelLabel() );
  cancelButton->setId( PkgNames::Cancel () );
  hSplit->addChild( cancelButton );

  solveButton = new NCPushButton( hSplit, opt, PkgNames::SolveLabel() );
  solveButton->setId( PkgNames::Solve () );
  hSplit->addChild( solveButton );

  okButton = new NCPushButton( hSplit, opt, PkgNames::OKLabel() );
  okButton->setId( PkgNames::OkButton () );
  hSplit->addChild( okButton );

}

///////////////////////////////////////////////////////////////////
//
//  checkDependencies
// 
//  creates local list of package dependencies
//
void NCPopupDeps::checkDependencies( )
{
    
    // FIXME: call something like Y2PM::packageManager().solve();

    // get some test pointer
    list<PMSelectablePtr> pkgList( Y2PM::packageManager().begin(), Y2PM::packageManager().end() );

    list<PMSelectablePtr>::iterator listIt = pkgList.begin();

    unsigned int i;
    PMObjectPtr pkgPtr;
    PMObjectPtr conflPtr;
    list<PMObjectPtr> conflList;

    NCMIL << "SOLVING" << endl;
    
    conflicts.clear();
    
    // TEST 
    for ( i = 0; listIt != pkgList.end(), i < 20;   ++listIt, i++ )
    {
	if ( (*listIt)->installedObj() )
	{
	    pkgPtr = (*listIt)->installedObj();
	}
	else if ( (*listIt)->candidateObj() )
	{
	    pkgPtr =  (*listIt)->candidateObj();
	}
	++listIt;
	if ( (*listIt)->installedObj() )
	{
	    conflPtr = (*listIt)->installedObj();
	}
	else if ( (*listIt)->candidateObj() )
	{
	    conflPtr =  (*listIt)->candidateObj();
	}
	conflList.push_back( conflPtr );
	++listIt;
	if ( (*listIt)->installedObj() )
	{
	    conflPtr = (*listIt)->installedObj();
	}
	else if ( (*listIt)->candidateObj() )
	{
	    conflPtr =  (*listIt)->candidateObj();
	}
	
	conflList.push_back( conflPtr );
	
	conflicts[pkgPtr] = conflList; 

    }

    // create the list of packages which have unresolved deps
    map<PMObjectPtr, list<PMObjectPtr> >::iterator it = conflicts.begin();
    list<PMObjectPtr> badPkgs; 
    
    while ( it != conflicts.end() )
    {
	badPkgs.push_back( (*it).first );
	++it;
    }

    fillDepsPackageList( pkgs, badPkgs );

}

bool NCPopupDeps::fillDepsPackageList( NCPkgTable * table, list<PMObjectPtr> badPkgs )
{
    list<PMObjectPtr>::iterator it = badPkgs.begin();
    unsigned int i;
    vector<string> pkgLine;
    pkgLine.reserve(4);
    
    if ( !table )
	return false;
    
    table->itemsCleared ();

    for ( i = 0, it = badPkgs.begin(); it != badPkgs.end();  ++it, i++ )    
    {
	pkgLine.clear();

	pkgLine.push_back( (*it)->getSelectable()->name() );	// package name
	pkgLine.push_back( (*it)->version() );	// version
	pkgLine.push_back( (*it)->summary() );  // short description
	FSize size = (*it)->size();     	// installed size
	pkgLine.push_back( size.asString() );

	table->addLine( (*it)->getSelectable()->status(), //  get the package status
			pkgLine,
			i,		// the index
			(*it) );	// the corresponding package pointer
	
    }

    return true;
}


void NCPopupDeps::concretelyDependency( PMObjectPtr pkgPtr )
{
    list<PMObjectPtr> conflList;
    
    // search for package in conflicts map
     map<PMObjectPtr, list<PMObjectPtr> >::iterator it = conflicts.find( pkgPtr );
     if ( it != conflicts.end() )
     {
	 // found a conflict dependeny for this package
	 conflList = (*it).second; 
     }

     // fill dependency table

     if ( !conflList.empty() )
     {
	 fillDepsPackageList( deps, conflList );
     }
    
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopupDeps::showDependencyPopup
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
NCursesEvent NCPopupDeps::showDependencyPopup( bool solve )
{
    if ( solve )
    {
	checkDependencies();
    }
    
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
    return ( dim == YD_HORIZ ? 50 : 20 );
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
	checkDependencies();
    }
    else if  ( currentId->compare( PkgNames::OkButton () ) == YO_EQUAL )
    {
	return false;
    }

    if ( postevent == NCursesEvent::cancel )
    {
	// return false means: close the popup dialog
	return false;
    }
    return true;
}

