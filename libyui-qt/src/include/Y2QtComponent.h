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

  File:	      Y2QtComponent.h

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/


#ifndef Y2QtComponent_h
#define Y2QtComponent_h

#include "Y2.h"

class YUIQt;

/**
 * @short YaST2 Component: Qt user interface
 * The YaST2 Component realizes a Qt based user interface with an
 * embedded YCP interpreter. It component name is "qt".
 */
class Y2QtComponent : public Y2Component
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
    YUIQt *interpreter;

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
    Y2QtComponent();

    /**
     * Cleans up.
     */
    ~Y2QtComponent();

    /**
     * The name of this component is qt.
     */
    string name() const;

    /**
     * Implements the server. The interpreter is created here and not
     * in the constructor, because in the meantime the server options
     * may have been set.
     */
    YCPValue evaluate(const YCPValue & command);

    /**
     * Is called by the genericfrontend, when the session is finished.
     * Close the user interace here.
     */
    void result(const YCPValue & result);

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

#endif	// Y2QtComponent_h

