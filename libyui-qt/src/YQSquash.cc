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

  File:	      YQSquash.cc

  Author:     Mathias Kettner <kettner@suse.de>
  Maintainer: Stefan Hundhammer <sh@suse.de>

/-*/


#include "YUIQt.h"
#include "YQSquash.h"

YQSquash::YQSquash( QWidget * parent, YWidgetOpt & opt,
		   bool hsquash, bool vsquash)
    : QWidget(parent)
    , YSquash(opt, hsquash, vsquash)
{
    setWidgetRep( this );
}


void YQSquash::setEnabling(bool enabled)
{
    QWidget::setEnabled(enabled);
}


void YQSquash::setSize(long newWidth, long newHeight)
{
    resize(newWidth, newHeight);
    YSquash::setSize(newWidth, newHeight);
}

#include "YQSquash.moc.cc"
