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

  File:	      NCApplication.h

  Author:     Gabriele Mohr <gs@suse.de>

/-*/


#ifndef NCApplication_h
#define NCApplication_h

#include "YApplication.h"


class NCApplication: public YApplication
{
    
protected:

    friend class YNCursesUI;
    
    /**
     * Constructor.
     *
     * Use YUI::app() to get the singleton for this class.
     **/
    NCApplication();

    /**
     * Destructor.
     **/
    virtual ~NCApplication();

public:
    
    /**
     * Set language and encoding for the locale environment ($LANG).
     *
     * 'language' is the ISO short code ("de_DE", "en_US", ...).
     *
     * 'encoding' an (optional) encoding ("utf8", ...) that will be appended if
     *  present.
     *
     * Reimplemented from YApplication.
     **/
    virtual void setLanguage( const string & language,
			      const string & encoding = string() );
    
};


#endif // NCApplication_h
