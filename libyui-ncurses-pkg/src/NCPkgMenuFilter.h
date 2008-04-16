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

   Author:     Gabriele MOhr <gs@suse.de>

/-*/
#ifndef NCPkgMenuFilter_h
#define NCPkgMenuFilter_h

#include <string>

#include <YReplacePoint.h>
#include <YWidget.h>
#include "NCurses.h"
#include "NCi18n.h"
#include "NCMenuButton.h"
#include "NCPackageSelector.h"
#include "NCPkgPatchSearch.h"

#include "NCZypp.h"

class NCPkgMenuFilter : public NCMenuButton {

    NCPkgMenuFilter & operator=( const NCPkgMenuFilter & );
    NCPkgMenuFilter            ( const NCPkgMenuFilter & );

private:
    NCPackageSelector *pkg;
    
    NCPkgPatchSearch *searchPopup;
    
public:

    YItemCollection items;

    YMenuItem *installable;
    YMenuItem *installed;
    YMenuItem *allPatches;
    YMenuItem *recommended;	
    YMenuItem *security;
    YMenuItem *optional;
    YMenuItem *search;

    NCPkgMenuFilter (YWidget *parent, string label, NCPackageSelector *pkger);
    virtual ~NCPkgMenuFilter();

    void createLayout();

    bool handleEvent (const NCursesEvent & event);		

};

#endif
