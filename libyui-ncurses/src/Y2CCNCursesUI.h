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

   File:       Y2CCNCursesUI.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
// -*- c++ -*-

#ifndef _Y2CCNCursesUI_h
#define _Y2CCNCursesUI_h

#include <iosfwd>

#include <y2/Y2ComponentCreator.h>

/**
 * @short Y2ComponentCreator that can create ncursesui user interfaces
 * A Y2ComponentCreator is an object, that can create components.
 * It is given a component name and - if it knows how to create
 * such a component - returns a newly created component of this
 * type. The Y2CCNCursesUI can create components with the name "ncursesui".
 */
class Y2CCNCursesUI : public Y2ComponentCreator
{
public:
   /**
    * Creates a NCursesUI component creator
    */
   Y2CCNCursesUI();

   /**
    * Returns true, since the NCursesUI component is a
    * YaST2 server.
    */
   bool isServerCreator() const;

   /**
    * Creates a new NCursesUI Userinterface component, if it
    * is asked to.
    * @param name Name of the component to create. Only
    * if this is "NCursesUI", the NCursesUI component will be created.
    * Otherwise 0 is returned.
    */
   Y2Component *create(const char *name) const;
};

#endif // Y2CCNCursesUI_h
