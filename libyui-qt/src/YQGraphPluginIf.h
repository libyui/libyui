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

  File:		YQGraphPluginIf.h

  Author:	Arvin Schnell <aschnell@suse.de>

/-*/

#ifndef YQGraphPluginIf_h
#define YQGraphPluginIf_h

#include <YGraph.h>
#include <YGraphPlugin.h>


class YQGraphPluginIf
{

public:

    virtual ~YQGraphPluginIf() {}

    virtual YGraph *createGraph( YWidget *parent, const string & filename,
				 const string & layoutAlgorithm ) = 0;

    virtual YGraph *createGraph( YWidget *parent, graph_t * graph ) = 0;

};

#endif
