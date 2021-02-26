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
    NCProblemSelectionBox (YWidget * parent, const string & label,
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

protected:
    virtual NCursesEvent wHandleInput( wint_t ch );
    
public:
    NCSolutionSelectionBox (YWidget * parent, const string & label,
			    NCPkgPopupDeps * aDepsPopup)
	: NCMultiSelectionBox( parent, label)
	, depsPopup (aDepsPopup) {}

    virtual ~NCSolutionSelectionBox () {}
};

using namespace std;

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

  // FIXME
  // opt.vWeight.setValue( 40 );

  vSplit->setNotify( true );

  new NCSpacing( vSplit, YD_VERT, false, 1 );

  head = new NCLabel( vSplit, "", true, false );	// isHeading = true

  new NCSpacing( vSplit, YD_VERT, false, 1 );

  // add the list containing packages with unresolved dependencies
  problemw = new NCProblemSelectionBox( vSplit, _("&Problems"),	this);
  problemw->setStretchable( YD_HORIZ, true );

  new NCSpacing( vSplit, YD_VERT, false, 1 );	

  NCAlignment * left = new NCAlignment( vSplit, YAlignBegin, YAlignUnchanged );

   // heading = false, outputField = true
  details =  new NCLabel ( left,"", false, true );
			       
  new NCSpacing( vSplit, YD_VERT, false, 1 );	// stretchable = true
  
  // add the package list containing the dependencies
  solutionw = new NCSolutionSelectionBox ( vSplit, _("Possible &Solutions"), this);
  
  new NCSpacing( vSplit, YD_VERT, false, 1 );	// stretchable = false
  
  NCLayoutBox * hSplit = new NCLayoutBox( vSplit, YD_HORIZ );


  // add the solve button
  solveButton = new NCPushButton( hSplit, NCPkgStrings::SolveLabel() );
  solveButton->setFunctionKey( 10 );

  new NCSpacing( hSplit, YD_HORIZ, true, 0.2 ); 	// stretchable = true

  // add the cancel button
  cancelButton = new NCPushButton( hSplit, NCPkgStrings::CancelLabel() );
  cancelButton->setFunctionKey( 9 );
  
  new NCSpacing( vSplit, YD_VERT, false, 1 );	// stretchable = false
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
    info->setNiceSize( 18, 4 );
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
    yuiMilestone() << "PROBLEMS" << endl;
    for (i = b, idx = 0; i != e; ++i, ++idx)
    {
	yuiMilestone() << "PROB " << (*i)->description () << endl;
	yuiMilestone() << ":    " << (*i)->details () << endl;

	// no solution yet
	problems.push_back (make_pair (*i, zypp::ProblemSolution_Ptr ()));
	//problemw->itemAdded ((*i)->description (), idx, false /*selected*/);
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
    for (ii = bb; ii != ee; ++ii) {
	yuiMilestone() << " SOL  " << (*ii)->description () << endl;
	yuiMilestone() << " :    " << (*ii)->details () << endl;

	solutionw->addItem( new YItem ( (*ii)->description(),		// label
					(user_solution == *ii) ) );	// selected
	
	yuiMilestone() << "usr: " << user_solution << " cur: " << *ii << endl;
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
    return NCurses::cols()-15;
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

    problems[prob_num] = make_pair (problem, sol);
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
	default: {
	    break;
	}
    }

    return ret;
}

