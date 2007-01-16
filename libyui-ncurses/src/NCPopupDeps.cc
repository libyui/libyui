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
#include "YDialog.h"
#include "NCSplit.h"
#include "NCSpacing.h"
#include "PkgNames.h"
#include "NCSelectionBox.h"
#include "NCMultiSelectionBox.h"
#include "NCPushButton.h"
#include "NCPopupInfo.h"
#include "NCTextEntry.h"

#include "NCi18n.h"

/*
  Textdomain "packages"
*/

class NCProblemSelectionBox : public NCSelectionBox
{
    typedef NCProblemSelectionBox Self;
    NCProblemSelectionBox (const Self &); // prohibit copying
    Self & operator= (const Self &); // prohibit assignment

    NCPopupDeps * depsPopup;	// to notify about changes
protected:
    virtual NCursesEvent wHandleInput( wint_t ch );
public:
    NCProblemSelectionBox (NCWidget * parent, const YWidgetOpt & opt,
			   const YCPString & label, NCPopupDeps * aDepsPopup)
	: NCSelectionBox (parent, opt, label), depsPopup (aDepsPopup) {}
    virtual ~NCProblemSelectionBox () {}
};

class NCSolutionSelectionBox : public NCMultiSelectionBox
{
    typedef NCSolutionSelectionBox Self;
    NCSolutionSelectionBox (const Self &); // prohibit copying
    Self & operator= (const Self &); // prohibit assignment

    NCPopupDeps * depsPopup;
protected:
    virtual NCursesEvent wHandleInput( wint_t ch );
public:
    NCSolutionSelectionBox (NCWidget * parent, const YWidgetOpt & opt,
			    const YCPString & label, NCPopupDeps * aDepsPopup)
	: NCMultiSelectionBox (parent, opt, label), depsPopup (aDepsPopup) {}
    virtual ~NCSolutionSelectionBox () {}
};

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

  vSplit->addChild( new NCSpacing( vSplit, opt, 0.8, false, true ) );

  // add the headline
  opt.isHeading.setValue( true );

  head = new NCLabel( vSplit, opt, YCPString("") );
  vSplit->addChild( head );

  vSplit->addChild( new NCSpacing( vSplit, opt, 0.4, false, true ) );

  // add the list containing packages with unresolved dependencies
  problemw = new NCProblemSelectionBox( vSplit, opt,
					_("&Problems"),
					this);
  vSplit->addChild( problemw );
  opt.isHStretchable.setValue( true );
  opt.isHeading.setValue( false );

  //vSplit->addChild( new NCSpacing( vSplit, opt, 0.2, false, true ) );

  details =  new NCTextEntry( vSplit, opt,
			      YCPString( "" ), YCPString( "" ),
			      200, 200 );

  vSplit->addChild( details );
  vSplit->addChild( new NCSpacing( vSplit, opt, 0.8, false, true ) );
    
  // add the package list containing the dependencies
  solutionw = new NCSolutionSelectionBox ( vSplit, opt,
					   _("P&ossible Solutions"),
					   this);
  vSplit->addChild( solutionw );
  
  opt.isHStretchable.setValue( false );
  opt.isHeading.setValue( true );
  
  vSplit->addChild( new NCSpacing( vSplit, opt, 0.6, false, true ) );
  
  NCSplit * hSplit = new NCSplit( vSplit, opt, YD_HORIZ );
  vSplit->addChild( hSplit );

  opt.isHStretchable.setValue( true );

  // add the solve button
  opt.key_Fxx.setValue( 10 );
  solveButton = new NCPushButton( hSplit, opt, YCPString(PkgNames::SolveLabel()) );
  solveButton->setId( PkgNames::Solve () );
  hSplit->addChild( solveButton );

  hSplit->addChild( new NCSpacing( hSplit, opt, 0.2, true, false ) );

  // add the cancel button
  opt.key_Fxx.setValue( 9 );
  cancelButton = new NCPushButton( hSplit, opt, YCPString(PkgNames::CancelLabel()) );
  cancelButton->setId( PkgNames::Cancel () );
  hSplit->addChild( cancelButton );

}

///////////////////////////////////////////////////////////////////
//
//  showDependencies
// 
//
bool NCPopupDeps::showDependencies( NCPkgSolverAction action )
{
    if ( !problemw )
	return true;
    
    bool cancel = false;
    
    // set headline and table type
    if ( head )
	head->setLabel( YCPString(PkgNames::PackageDeps()) );

    // evaluate the result and fill the list with packages
    // which have unresolved deps
    bool success = solve (problemw, action );

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


bool NCPopupDeps::solve( NCSelectionBox * problemw, NCPkgSolverAction action )
{
    if ( !problemw )
	return false;

    NCDBG << "Solving..." << endl ;

    NCPopupInfo info( wpos(10, 10),  YCPString( "" ),
		      YCPString(PkgNames::Solving()),
		      PkgNames::OKLabel() );
    info.setNiceSize( 18, 4 );
    info.popup();
    
    zypp::Resolver_Ptr resolver = zypp::getZYpp()->resolver();

    bool success = false;
    switch ( action )
    {
	case S_Solve:
	    success = resolver->resolvePool();
	    break;
	case S_Verify:
	    success = resolver->verifySystem( true ); // check hardware
	    break;
	default:
	    NCERR << "Unknown action for resolve" << endl;
    }
    
    info.popdown();

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
    UIMIL << "PROBLEMS" << endl;
    for (i = b, idx = 0; i != e; ++i, ++idx)
    {
	UIMIL << "PROB " << (*i)->description () << endl;
	UIMIL << ":    " << (*i)->details () << endl;

	// no solution yet
	problems.push_back (make_pair (*i, zypp::ProblemSolution_Ptr ()));
	problemw->itemAdded ((*i)->description (), idx, false /*selected*/);
    }

    return false;
}

bool NCPopupDeps::showSolutions( int index )
{
    if (!solutionw)
	return false;
    
    unsigned int size = problems.size ();
    solutionw->clearItems ();
    
    if ( index < 0 || (unsigned int)index >= size )
	return false;

    YCPList items;


    zypp::ResolverProblem_Ptr problem = problems[index].first;
    zypp::ProblemSolution_Ptr user_solution = problems[index].second;

    details->setText( YCPString(problem->details()) );
    details->setCurPos( 0 );
    
    zypp::ProblemSolutionList solutions = problem->solutions ();
    zypp::ProblemSolutionList::iterator
	bb = solutions.begin (),
	ee = solutions.end (),
	ii;
    for (ii = bb; ii != ee; ++ii) {
	UIMIL << " SOL  " << (*ii)->description () << endl;
	UIMIL << " :    " << (*ii)->details () << endl;

	YCPList termargs;
	termargs->add (YCPString ((*ii)->description ())); // label
	termargs->add (YCPBoolean (user_solution == *ii)); // selected
	UIMIL << "usr: " << user_solution << " cur: " << *ii << endl;

	items->add (YCPTerm (YUISymbol_item, termargs));
    }
    solutionw->changeWidget (YCPSymbol (YUIProperty_Items),
			     items);    
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
NCursesEvent NCPopupDeps::showDependencyPopup( NCPkgSolverAction action )
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
NCursesEvent NCPopupDeps::wHandleInput( wint_t ch )
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
bool NCPopupDeps::postAgain( NCPkgSolverAction action )
{
    if ( ! postevent.widget )
	return false;

    YCPValue currentId =  dynamic_cast<YWidget *>(postevent.widget)->id();

    if ( currentId.isNull() )
	return false;
    
    if ( currentId->compare( PkgNames::Cancel () ) == YO_EQUAL )
    {
	// close the dialog 
	postevent = NCursesEvent::cancel;
    }
    else if ( currentId->compare( PkgNames::Solve () ) == YO_EQUAL )
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
//	METHOD NAME : NCPopupDeps::setSolution
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
void NCPopupDeps::setSolution (int index)
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
	    int cur = getCurrentItem ();
	    bool on = itemIsSelected (cur);
	    if (on)
	    {
		deselectAllItems ();
		selectItem (cur);
		depsPopup->setSolution (cur);
	    }
	    break;	
	}
	default: {
	    break;
	}
    }

    return ret;
}

