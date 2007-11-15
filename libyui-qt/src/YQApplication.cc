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

  File:	      YQApplication.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#include "YQApplication.h"


YQApplication::YQApplication()
{
    setIconBasePath( ICONDIR "/icons/22x22/apps/" );
}


YQApplication::~YQApplication()
{
    
}


#include "YQApplication.moc"
