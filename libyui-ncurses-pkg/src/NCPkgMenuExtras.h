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

   File:       NCPkgMenuExtras.cc

   Author:     Hedgehog Painter <kmachalkova@suse.cz>

/-*/
#ifndef NCPkgMenuExtras_h
#define NCPkgMenuExtras_h

#include <string>

#include "NCurses.h"
#include "NCi18n.h"
#include "NCMenuButton.h"
//#include "NCPackageSelector.h"
#include "NCPkgStrings.h"
#include "NCPkgPopupDiskspace.h"


class NCPackageSelector;


class NCPkgMenuExtras : public NCMenuButton {

    NCPkgMenuExtras & operator=( const NCPkgMenuExtras & );
    NCPkgMenuExtras            ( const NCPkgMenuExtras & );

public:

    YItemCollection items;

    YMenuItem *exportFile;
    YMenuItem *importFile;
    YMenuItem *diskSpace;

    NCPackageSelector *pkg;

    NCPkgMenuExtras (YWidget *parent, string label, NCPackageSelector *pkger);
    virtual ~NCPkgMenuExtras();

    void createLayout();

    bool handleEvent (const NCursesEvent & event);

    void importSelectable ( ZyppSel selectable, bool isWanted, const char*kind );
 
    bool exportToFile();

    bool importFromFile();

    bool showDiskSpace();		

};

#endif
