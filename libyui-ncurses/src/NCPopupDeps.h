/*-----------------------------------------------------------*- c++ -*-\
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

   File:       NCPopupDeps.h

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCPopupDeps_h
#define NCPopupDeps_h

#include <iosfwd>

#include <vector>
#include <string>

#include "NCPopup.h"
#include "NCLabel.h"
#include <zypp/Resolver.h>

#include "YQZypp.h"

class NCPushButton;
class NCSelectionBox;
class NCMultiSelectionBox;
class PackageSelector;


namespace PkgDep {
    class ErrorResult;
    class ErrorResultList;
    class ResultList;
};

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPopupDeps
//
//	DESCRIPTION :
//
class NCPopupDeps : public NCPopup {

    NCPopupDeps & operator=( const NCPopupDeps & );
    NCPopupDeps            ( const NCPopupDeps & );

private:

    typedef std::vector<std::pair<
	zypp::ResolverProblem_Ptr,
	zypp::ProblemSolution_Ptr> > ProblemSolutionCorrespondence;
    // indexed by widget position,
    // keeps the user selected solution (or 0) for each problem
    ProblemSolutionCorrespondence problems;

    NCPushButton * cancelButton;
    NCPushButton * solveButton;		
    
    NCMultiSelectionBox * solutionw; // resolver problem solutions

    NCLabel * head;			// the headline
    
    PackageSelector * packager;		// connection to the package selector
    
    void createLayout();
    
protected:

    NCSelectionBox * problemw;	// resolver problems
    
    virtual bool postAgain();

    virtual NCursesEvent wHandleInput( wint_t ch );
    
public:

    NCPopupDeps( const wpos at, PackageSelector * pkger );
    virtual ~NCPopupDeps();

    virtual long nicesize(YUIDimension dim);

    NCursesEvent showDependencyPopup( );

    bool showDependencies( );
    
    bool solve( NCSelectionBox * problemw );

    bool showSolutions( int index );
    // for the currently selected problem, choose this solution
    void setSolution (int index);
};

///////////////////////////////////////////////////////////////////


#endif // NCPopupDeps_h
