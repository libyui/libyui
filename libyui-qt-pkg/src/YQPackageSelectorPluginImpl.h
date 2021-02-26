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

  File:		YQPackageSelectorPluginImpl.h

  Author:	Hedgehog Painter <kmachalkova@suse.cz>


/-*/

#ifndef YQPackageSelectorPluginImpl_h
#define YQPackageSelectorPluginImpl_h

#include "YQPackageSelectorPluginIf.h"

class YQPackageSelectorPluginImpl : public YQPackageSelectorPluginIf
{

  public:

    virtual ~YQPackageSelectorPluginImpl() {}

    virtual YPackageSelector * createPackageSelector( YWidget *	parent, long modeFlags);

    virtual YWidget *createPatternSelector( YWidget * parent, long modeFlags );

    virtual YWidget *createSimplePatchSelector( YWidget * parent, long modeFlags );
};
#endif
