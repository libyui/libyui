/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|						   (c) SuSE Linux GmbH |
\----------------------------------------------------------------------/

  File:		YQGraphPluginImpl.h

  Author:	Arvin Schnell <aschnell@suse.de>

/-*/

#ifndef YQGraphPluginImpl_h
#define YQGraphPluginImpl_h

#include "YQGraphPluginIf.h"

class YQGraphPluginImpl : public YQGraphPluginIf
{

public:

    virtual ~YQGraphPluginImpl() {}

    virtual YGraph * createGraph( YWidget * parent, const string & filename,
				  const string & layoutAlgorithm );

    virtual YGraph * createGraph( YWidget * parent, graph_t * graph );

};

#endif
