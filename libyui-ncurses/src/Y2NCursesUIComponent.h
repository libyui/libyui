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

   File:       Y2NCursesUIComponent.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
// -*- c++ -*-

#ifndef Y2NCursesUIComponent_h
#define Y2NCursesUIComponent_h

#include <iosfwd>

#include <Y2.h>

class YUIInterpreter;

/**
 * @short YaST2 Component: NCursesUI user interface
 * The YaST2 Component realizes a NCursesUI based user interface with an
 * embedded YCP interpreter.
 */
class Y2NCursesUIComponent : public Y2Component
{
   /**
    * Used to store the server options until the server is launched
    */
   int argc;

   /**
     * Used to store the server options until the server is launched
     */
   char **argv;

    /**
     * Does the actual work.
     */
    YUIInterpreter *interpreter;

    /**
     * since we're defining our own setCallback/getCallback function
     * we must save the callback pointer ourselfs. See Y2Component.
     */
    Y2Component *m_callback;

    /**
     * This is false, if not threads should be used.
     */
    bool with_threads;

public:
   /**
    * Initialize data.
     */
   Y2NCursesUIComponent();

   /**
     * Cleans up.
     */
   ~Y2NCursesUIComponent();

   /**
     * The name of this component
     */
   string name() const;
   /**
     * Implements the server. The interpreter is created here and not
     * in the constructor, because in the meantime the server options
     * may have been set.
     */
   YCPValue evaluate(const YCPValue &command);

   /**
     * Is called by the genericfrontend, when the session is finished.
     * Close the user interace here.
     */
   void result(const YCPValue &result);

   /**
     * Is called by the genericfrontend after it parsed the commandline.
     * gives the QT UI its commandline options. We store it here and
     * wait until we create the interpreter in @ref #evaluate.
     */
   void setServerOptions(int argc, char **argv);

    /**
     * Functions to pass callback information
     * The callback is a pointer to a Y2Component with
     * a valid evaluate() function.
     */

    Y2Component *getCallback (void) const;
    void setCallback (Y2Component *callback);

};

#endif // Y2NCursesUIComponent_h
