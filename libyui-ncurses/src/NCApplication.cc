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

  File:	      NCApplication.cc

  Author:     Gabriele Mohr <gs@suse.de>

/-*/


#include "Y2Log.h"
#include "NCurses.h"
#include "NCApplication.h"


NCApplication::NCApplication()
{
    
}


NCApplication::~NCApplication()
{
    
}


void
NCApplication::setLanguage( const string & language,
			    const string & encoding )
{
    YApplication::setLanguage( language, encoding );
    NCurses::Refresh();
    
    NCDBG << "Language: " << language << " Encoding: " << ((encoding!="")?encoding:"NOT SET") << endl;
  
}


