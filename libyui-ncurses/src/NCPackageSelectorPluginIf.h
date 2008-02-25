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

  File:		NCPackageSelectorPluginIf.h

  Author:	Hedgehog Painter <kmachalkova@suse.cz>


/-*/

#ifndef NCPackageSelectorPluginIf_h
#define NCPackageSelectorPluginIf_h

#include <YDialog.h>
#include <YEvent.h>
#include <YPackageSelectorPlugin.h>

class NCPackageSelectorPluginIf 
{

  public:

    virtual ~NCPackageSelectorPluginIf() {}

    virtual YPackageSelector *createPackageSelector( YWidget *parent, long modeFlags ) = 0 ;

    virtual YEvent *runPkgSelection (YDialog *currentDialog, YWidget *packageSelector ) = 0;

    virtual YWidget *createPkgSpecial (YWidget *parent, const string &subwidget ) = 0;

};
#endif
