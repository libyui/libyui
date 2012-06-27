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

   File:       NCPkgPopupDeps.cc

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Bubli <kmachalkova@suse.cz>

/-*/
#define YUILogComponent "ncurses-pkg"
#include <YUILog.h>

#include "NCPkgPopupDeps.h"

#include "NCAlignment.h"
#include "NCTree.h"
#include "YDialog.h"
#include "NCLayoutBox.h"
#include "NCSpacing.h"
#include "NCPkgStrings.h"
#include "NCSelectionBox.h"
#include "NCMultiSelectionBox.h"
#include "NCPushButton.h"
#include "NCPopupInfo.h"
#include "NCInputField.h"

#include "NCi18n.h"

using std::endl;

/*
  Textdomain "ncurses-pkg"
*/

class NCProblemSelectionBox : public NCSelectionBox
{
    typedef NCProblemSelectionBox Self;
    NCProblemSelectionBox (const Self &); // prohibit copying
    Self & operator= (const Self &); // prohibit assignment

    NCPkgPopupDeps * depsPopup;	// to notify about changes

protected:
    virtual NCursesEvent wHandleInput( wint_t ch );

public:
    NCProblemSelectionBox (YWidget * parent, const std::string & label,
			   NCPkgPopupDeps * aDepsPopup)
	: NCSelectionBox( parent, label),
	  depsPopup (aDepsPopup) {}

    virtual ~NCProblemSelectionBox () {}
};

class NCSolutionSelectionBox : public NCMultiSelectionBox
{
    typedef NCSolutionSelectionBox Self;
    NCSolutionSelectionBox (const Self &); // prohibit copying
    Self & operator= (const Self &); // prohibit assignment

    NCPkgPopupDeps * depsPopup;
    std::map<YItem *, std::string> detailsMap;

protected:
    virtual NCursesEvent wHandleInput( wint_t ch );

public:
    NCSolutionSelectionBox (YWidget * parent, const std::string & label,
			    NCPkgPopupDeps * aDepsPopup)
	: NCMultiSelectionBox( parent, label)
	, depsPopup (aDepsPopup) {}

    virtual ~NCSolutionSelectionBox () {}
    void saveDetails( YItem * item, std::string details ) { detailsMap[item] = details; }
};


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupDeps::NCPkgPopupDeps
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCPkgPopupDeps::NCPkgPopupDeps( const wpos at, NCPackageSelector * pkger )
    : NCPopup( at, false )
      , cancelButton( 0 )
      , solveButton( 0 )
      , solutionw( 0 )
      , head( 0 )
      , details( 0 )
      , solDetails( 0 )
      , packager( pkger )
      , problemw( 0 )

{
    createLayout();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupDeps::~NCPkgPopupDeps
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCPkgPopupDeps::~NCPkgPopupDeps()
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
void NCPkgPopupDeps::createLayout( )
{

  // vertical split is the (only) child of the dialog
  NCLayoutBox * vSplit = new NCLayoutBox( this, YD_VERT );

  vSplit->setNotify( true );

  new NCSpacing( vSplit, YD_VERT, false, 1 );

  head = new NCLabel( vSplit, "", true );	// isHeading = true

  // only add spacings if there's enough space
  if ( this->preferredHeight() > 25 )
      new NCSpacing( vSplit, YD_VERT, false, 1 );

  NCAlignment * left = new NCAlignment( vSplit, YAlignBegin, YAlignUnchanged );
  left->setWeight(YD_VERT, 30 );

  // the list containing the problems (the unresolved package dependencies)
  problemw = new NCProblemSelectionBox( left, _( "&Problems" ),	this);
  problemw->setStretchable( YD_HORIZ, true );

  NCAlignment * left1 = new NCAlignment( vSplit, YAlignBegin, YAlignUnchanged );
  left1->setWeight(YD_VERT, 10 );

  // show the details of the problem
  details =  new NCLabel ( left1, "", false, true );	// heading = false,
  details->setStretchable( YD_HORIZ, true );		// outputField = true

  if ( this->preferredHeight() > 25 )
      new NCSpacing( vSplit, YD_VERT, false, 0.5 );	// stretchable = false

  NCAlignment * left2 = new NCAlignment( vSplit, YAlignBegin, YAlignUnchanged );
  left2->setWeight( YD_VERT, 30 );

  // the list containing the solutions of a dependency problem
  solutionw = new NCSolutionSelectionBox ( left2, _( "Possible &Solutions" ), this);

  if ( this->preferredHeight() > 25 )
      new NCSpacing( vSplit, YD_VERT, false, 1 );

  NCAlignment * left3 = new NCAlignment( vSplit, YAlignBegin, YAlignUnchanged );
  left3->setWeight( YD_VERT, 30 );

  // show the details of the solution
  solDetails = new NCRichText ( left3, "", true );	// plain text mode = true

  if ( this->preferredHeight() > 25 )
      new NCSpacing( vSplit, YD_VERT, false, 1 );	// stretchable = false

  NCLayoutBox * hSplit = new NCLayoutBox( vSplit, YD_HORIZ );

  // add the solve button
  solveButton = new NCPushButton( hSplit, NCPkgStrings::SolveLabel() );
  solveButton->setFunctionKey( 10 );

  new NCSpacing( hSplit, YD_HORIZ, true, 0.2 ); 	// stretchable = true

  // add the cancel button
  cancelButton = new NCPushButton( hSplit, NCPkgStrings::CancelLabel() );
  cancelButton->setFunctionKey( 9 );

  if ( this->preferredHeight() > 25 )
   new NCSpacing( vSplit, YD_VERT, false, 0.5 );	// stretchable = false
}

///////////////////////////////////////////////////////////////////
//
//  showDependencies
//
//
bool NCPkgPopupDeps::showDependencies( NCPkgSolverAction action, bool * ok )
{
    if ( !problemw )
	return true;

    bool cancel = false;

    // set headline and table type
    if ( head )
	head->setLabel( NCPkgStrings::PackageDeps() );

    // evaluate the result and fill the list with packages
    // which have unresolved deps
    bool success = solve (problemw, action );
    *ok = success;

    if (!success)
    {
	// show first dependency
	showSolutions( problemw->getCurrentItem() );
	NCursesEvent input = showDependencyPopup( action );    // show the dependencies

	if ( input == NCursesEvent::cancel
	     && input.detail != NCursesEvent::USERDEF )
	{
	    cancel = true;
	}
	problemw->setKeyboardFocus();
    }

    return cancel;
}


bool NCPkgPopupDeps::solve( NCSelectionBox * problemw, NCPkgSolverAction action )
{
    if ( !problemw )
	return false;

    yuiDebug() << "Solving..." << endl;

    NCPopupInfo * info = new NCPopupInfo( wpos( (NCurses::lines()-4)/2, (NCurses::cols()-18)/2 ),
					  "",
					  NCPkgStrings::Solving(),
					  NCPkgStrings::OKLabel()
					  );
    info->setPreferredSize( 18, 4 );
    info->popup();

    zypp::Resolver_Ptr resolver = zypp::getZYpp()->resolver();

    bool success = false;
    switch ( action )
    {
	case S_Solve:
	    success = resolver->resolvePool();
	    break;
	case S_Verify:
	    success = resolver->verifySystem( ); // check hardware
	    break;
	default:
	    yuiError() << "Unknown action for resolve" << endl;
    }

    info->popdown();

    YDialog::deleteTopmostDialog();

    if (success)
	return true;

    // clear list
    problems.clear ();
    problemw->deleteAllItems ();

    zypp::ResolverProblemList rproblems = resolver->problems ();
    zypp::ResolverProblemList::iterator
	b = rproblems.begin (),
	e = rproblems.end (),
	i;
    int idx;

    for (i = b, idx = 0; i != e; ++i, ++idx)
    {
	yuiMilestone() << "Problem: " << (*i)->description () << endl;
	yuiMilestone() << "Details: " << (*i)->details () << endl;

	// no solution yet
	problems.push_back (std::make_pair (*i, zypp::ProblemSolution_Ptr ()));

	problemw->addItem( (*i)->description(), false );	// selected: false
    }

    return false;
}

bool NCPkgPopupDeps::showSolutions( int index )
{
    if (!solutionw)
	return false;

    unsigned int size = problems.size ();

    if ( index < 0 || (unsigned int)index >= size )
	return false;

    solutionw->startMultipleChanges();
    solutionw->deleteAllItems();

    zypp::ResolverProblem_Ptr problem = problems[index].first;
    zypp::ProblemSolution_Ptr user_solution = problems[index].second;

    details->setText( problem->details() );

    zypp::ProblemSolutionList solutions = problem->solutions ();
    zypp::ProblemSolutionList::iterator
	bb = solutions.begin (),
	ee = solutions.end (),
	ii;

    bool showDetails = true;;
    std::string description;

    for (ii = bb; ii != ee; ++ii) {
	yuiMilestone() << "Solution:  " << (*ii)->description () << endl;
	yuiMilestone() << "Details:   " << (*ii)->details () << endl;
	yuiMilestone() << "User decision: " << user_solution << endl;

	description = (*ii)->description();

	if ( !((*ii)->details().empty()) )
	    // hint for the user: more information below
	    description += _( " see below" );

	if ( showDetails )
	{
	    showSolutionDetails( (*ii)->details() ); // show details of 1. solution
	    showDetails = false;
	}

	YItem *newItem = new YItem ( description,			// text
				     (user_solution == *ii) );		// selected ?

	solutionw->addItem( newItem );
	solutionw->saveDetails( newItem, (*ii)->details() );

	yuiDebug() << "Solution: " << (*ii) << endl;  // Complete info
    }

    solutionw->doneMultipleChanges();

    return true;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupDeps::showDependencyPopup
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
NCursesEvent NCPkgPopupDeps::showDependencyPopup( NCPkgSolverAction action )
{
    postevent = NCursesEvent();

    do {
	popupDialog();
    } while ( postAgain( action ) );

    popdownDialog();

    return postevent;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupDeps::preferredWidth
//	METHOD TYPE : int
//
//	DESCRIPTION :
//
int NCPkgPopupDeps::preferredWidth()
{
    return NCurses::cols()-8;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupDeps::preferredHeight
//	METHOD TYPE : int
//
//	DESCRIPTION :
//
int NCPkgPopupDeps::preferredHeight()
{
    return NCurses::lines()-5;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPopup::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCPkgPopupDeps::wHandleInput( wint_t ch )
{
    if ( ch == 27 ) // ESC
	return NCursesEvent::cancel;

    return NCDialog::wHandleInput( ch );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupDeps::postAgain
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCPkgPopupDeps::postAgain( NCPkgSolverAction action )
{
    if ( ! postevent.widget )
	return false;

    if ( postevent.widget == cancelButton )
    {
	// close the dialog
	postevent = NCursesEvent::cancel;
    }
    else if ( postevent.widget == solveButton )
    {
	// apply the solution here
	zypp::Resolver_Ptr resolver = zypp::getZYpp()->resolver();
	ProblemSolutionCorrespondence::iterator
	    b = problems.begin (),
	    e = problems.end (),
	    i;
	zypp::ProblemSolutionList solutions;
	for (i = b; i != e; ++i)
	{
	    // *i is std::pair< zypp::ResolverProblem_Ptr,
	    //                  zypp::ProblemSolution_Ptr >
	    if (i->second)
	    {
		solutions.push_back (i->second);
	    }
	}
	resolver->applySolutions (solutions);

	// and solve again
	bool success = solve (problemw, action );

	if ( !success )
	{
		problemw->setKeyboardFocus();
		showSolutions( problemw->getCurrentItem() );
	}
	else	// everything ok
	{
            // close the dialog
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


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCPkgPopupDeps::setSolution
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
void NCPkgPopupDeps::setSolution (int index)
{
    // we must search the list :( bad design here
    // but the solution list is short
    int prob_num = problemw->getCurrentItem ();
    zypp::ResolverProblem_Ptr problem = problems[prob_num].first;
    zypp::ProblemSolution_Ptr sol = zypp::ProblemSolution_Ptr ();

    zypp::ProblemSolutionList solutions = problem->solutions ();
    zypp::ProblemSolutionList::iterator
	bb = solutions.begin (),
	ee = solutions.end (),
	ii;
    int idx;
    for (ii = bb, idx = 0; ii != ee && idx < index; ++ii, ++idx) {
	//empty
    }
    if (ii != ee)
	sol = *ii;

    problems[prob_num] = std::make_pair (problem, sol);
}

void NCPkgPopupDeps::showSolutionDetails( std::string details )
{
    std::string text;
    if ( details.empty() )
	// hint for the user: there isn't any additional information
	// (for the currently selected  solution of a dependency problem)
	text = _( "No further solution details available" );
    else
	text = details;

    if ( solDetails )
	solDetails->setText( text );

}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCProblemSelectionBox::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCProblemSelectionBox::wHandleInput( wint_t key )
{
    NCursesEvent ret = NCursesEvent::none;

    // call handleInput of NCPad
    handleInput( key );

    switch ( key )
    {
	case KEY_UP:
	case KEY_DOWN:
	case KEY_NPAGE:
	case KEY_PPAGE:
	case KEY_END:
	case KEY_HOME: {
	    // show the corresponding information
	    depsPopup->showSolutions (getCurrentItem ());
	    ret = NCursesEvent::handled;
	    break;
	}
	default: {
//?
//	    ret = NCursesEvent::handled;
	    break;
	}
    }

    return ret;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCSolutionSelectionBox::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCSolutionSelectionBox::wHandleInput( wint_t key )
{
    NCursesEvent ret = NCMultiSelectionBox::wHandleInput( key );

    switch ( key )
    {
	case KEY_SPACE:
	case KEY_RETURN: {
	    // act like a radio button
	    // make sure that only one item is selected
	    YItem *cur = currentItem ();
	    bool on = isItemSelected( cur );
	    if (on)
	    {
		deselectAllItems ();
		selectItem (cur, true);
		depsPopup->setSolution ( cur->index() );
	    }
	    break;
	}
	case KEY_UP:
	case KEY_DOWN: {
	    // show details
	    depsPopup->showSolutionDetails( detailsMap[currentItem()] );
	    break;
	}

	default: {
	    break;
	}
    }

    return ret;
}

