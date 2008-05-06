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

   File:       NCPkgMenuFilter.cc

   Author:     Gabriele Mohr <gs@suse.de>

/-*/
#ifndef NCPkgMenuFilter_h
#define NCPkgMenuFilter_h

#include <string>

#include <YReplacePoint.h>
#include <YWidget.h>
#include "NCurses.h"
#include "NCi18n.h"
#include "NCMenuButton.h"
#include "NCPkgPatchSearch.h"

class NCPackageSelector;

#include "NCZypp.h"

class NCPkgMenuFilter : public NCMenuButton {

public:
	enum PatchFilter 
	{
	    F_Needed,
	    F_Unneeded,
	    F_All,
	    F_Recommended,
	    F_Security,
	    F_Optional,
	    F_Unknown
	};
    
    NCPkgMenuFilter & operator=( const NCPkgMenuFilter & );
    NCPkgMenuFilter            ( const NCPkgMenuFilter & );

private:
    NCPackageSelector *pkg;
    
    NCPkgPatchSearch *searchPopup;

    YItemCollection items;

    YMenuItem *needed;
    YMenuItem *unneeded;
    YMenuItem *allPatches;
    YMenuItem *recommended;	
    YMenuItem *security;
    YMenuItem *optional;
    YMenuItem *search;

public:
    
    NCPkgMenuFilter (YWidget *parent, string label, NCPackageSelector *pkger);
    virtual ~NCPkgMenuFilter();

    void createLayout();

    bool handleEvent (const NCursesEvent & event);		

};

#endif
