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

   File:       NCtoY2Event.h

   Authors:    Michael Andres <ma@suse.de>
	       Stefan Hundhammer <sh@suse.de>

/-*/

#ifndef NCtoY2Event_h
#define NCtoY2Event_h

#include <NCurses.h>	// NCursesEvent, iostream


/**
 * Helper class for translating an NCurses event to a YEvent.
 **/
class NCtoY2Event : public NCursesEvent
{
public:
    friend ostream & operator<<( std::ostream & stream, const NCtoY2Event & event );

    /**
     * Default constructor
     **/
    NCtoY2Event() {}

    /**
     * Constructor from an NCursesEvent
     **/
    NCtoY2Event( const NCursesEvent & ncev );

    /**
     * Assignment operator
     **/
    NCtoY2Event & operator=( const NCursesEvent & ncev );

    /**
     * The reason of existence of this class:
     * Translate the NCursesEvent to a YEvent.
     **/
    YEvent * propagate();
};


#endif // ifndef NCtoY2Event_h
