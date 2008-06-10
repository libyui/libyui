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

  File:		NCPackageSelectorPluginImpl.h

  Author:	Hedgehog Painter <kmachalkova@suse.cz>


/-*/ 

#ifndef NCPackageSelectorPluginImpl_h
#define NCPackageSelectorPluginImpl_h

#include "NCPackageSelectorPluginIf.h"

class NCPackageSelectorPluginImpl : public NCPackageSelectorPluginIf 
{

  public:

    virtual ~NCPackageSelectorPluginImpl() {};

    virtual YPackageSelector * createPackageSelector( YWidget *	parent, long modeFlags);

    virtual YEvent *runPkgSelection (YDialog *currentDialog, YWidget *packageSelector );

    virtual YWidget *createPkgSpecial (YWidget *parent, const string &subwidget );

};
#endif
