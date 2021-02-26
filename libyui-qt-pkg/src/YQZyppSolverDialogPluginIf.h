/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|						     (c) SuSE Linux AG |
\----------------------------------------------------------------------/

  File:		YQZyppSolverDialogPluginIf.h

  Author:	Stefan Schubert <schubi@suse.de>


/-*/

#ifndef YQZyppSolverDialogPluginIf_h
#define YQZyppSolverDialogPluginIf_h

#include <zypp/PoolItem.h>

class YQZyppSolverDialogPluginIf
{

  public:

    virtual ~YQZyppSolverDialogPluginIf() {}

    virtual bool createZyppSolverDialog(const zypp::PoolItem item) = 0 ;
};
#endif
