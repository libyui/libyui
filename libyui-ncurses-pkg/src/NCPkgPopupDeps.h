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
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:       NCPkgPopupDeps.h

   Author:     Gabriele Strattner <gs@suse.de>
   Maintainer: Bubli <kmachalkova@suse.cz>

/-*/
#ifndef NCPkgPopupDeps_h
#define NCPkgPopupDeps_h

#include <iosfwd>

#include <vector>
#include <string>

#include "NCPopup.h"
#include "NCLabel.h"
#include <zypp/Resolver.h>

#include "NCZypp.h"

class NCPushButton;
class NCSelectionBox;
class NCMultiSelectionBox;
class NCPackageSelector;
class NCRichText;
class NCSolutionSelectionBox;

namespace PkgDep
{
    class ErrorResult;
    class ErrorResultList;
    class ResultList;
};

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPkgPopupDeps
//
//	DESCRIPTION :
//
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
