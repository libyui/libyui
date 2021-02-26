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

  File:		YQPackageSelectorPluginIf.h

  Author:	Hedgehog Painter <kmachalkova@suse.cz>


/-*/

#ifndef YQPackageSelectorPluginIf_h
#define YQPackageSelectorPluginIf_h

#include <YDialog.h>
#include <YEvent.h>
#include <YPackageSelectorPlugin.h>

class YQPackageSelectorPluginIf
{

  public:

    virtual ~YQPackageSelectorPluginIf() {}

    virtual YPackageSelector *createPackageSelector( YWidget *parent, long modeFlags ) = 0 ;

    virtual YWidget *createPatternSelector( YWidget * parent, long modeFlags ) = 0;

    virtual YWidget *createSimplePatchSelector( YWidget * parent, long modeFlags ) = 0;
};
#endif
