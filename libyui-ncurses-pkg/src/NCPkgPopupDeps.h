/*
  Copyright (c) 2002-2011 Novell, Inc.
  Copyright (c) 2021 SUSE LLC

  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) version 3.0 of the License. This library
  is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
  License for more details. You should have received a copy of the GNU
  Lesser General Public License along with this library; if not, write
  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301 USA


  File:       NCPkgPopupDeps.h

  Authors:    Gabriele Strattner <gs@suse.de>
              Bubli <kmachalkova@suse.cz>

*/


#ifndef NCPkgPopupDeps_h
#define NCPkgPopupDeps_h

#include <iosfwd>
#include <string>
#include <vector>

#include <zypp/Resolver.h>

#include <yui/ncurses/NCPopup.h>
#include <yui/ncurses/NCLabel.h>

#include "NCZypp.h"


class NCPushButton;
class NCSelectionBox;
class NCPackageSelector;
class NCRichText;
class NCSolutionSelectionBox;


namespace PkgDep
{
    class ErrorResult;
    class ErrorResultList;
    class ResultList;
};


class NCPkgPopupDeps : public NCPopup
{
    NCPkgPopupDeps & operator=( const NCPkgPopupDeps & );
    NCPkgPopupDeps            ( const NCPkgPopupDeps & );

public:
    enum NCPkgSolverAction
    {
	S_Solve,
	S_Verify,
	S_Unknown
    };

private:

    typedef std::vector<std::pair<
	zypp::ResolverProblem_Ptr,
	zypp::ProblemSolution_Ptr> > ProblemSolutionCorrespondence;
    // indexed by widget position,
    // keeps the user selected solution (or 0) for each problem
    ProblemSolutionCorrespondence problems;

    NCPushButton * cancelButton;
    NCPushButton * solveButton;

    NCSolutionSelectionBox * solutionw; // resolver problem solutions

    NCLabel * head;			// the headline

    NCLabel *details;		// problem details
    NCRichText *solDetails;	// solution details

    NCPackageSelector * packager;	// connection to the package selector

    void createLayout();

protected:

    NCSelectionBox * problemw;	// resolver problems

    virtual bool postAgain( NCPkgSolverAction action );

    virtual NCursesEvent wHandleInput( wint_t ch );

public:

    NCPkgPopupDeps( const wpos at, NCPackageSelector * pkger );
    virtual ~NCPkgPopupDeps();

    virtual int preferredWidth();
    virtual int preferredHeight();

    NCursesEvent showDependencyPopup( NCPkgSolverAction action );

    bool showDependencies( NCPkgSolverAction action, bool * ok );

    bool solve( NCSelectionBox * problemw,  NCPkgSolverAction action );

    bool showSolutions( int index );
    // for the currently selected problem, choose this solution
    void setSolution (int index);
    // show details
    void showSolutionDetails( std::string details );
};

///////////////////////////////////////////////////////////////////


#endif // NCPkgPopupDeps_h
