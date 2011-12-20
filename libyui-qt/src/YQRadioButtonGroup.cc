
/* ****************************************************************************
  |
  | Copyright (c) 2000 - 2010 Novell, Inc.
  | All Rights Reserved.
  |
  | This program is free software; you can redistribute it and/or
  | modify it under the terms of version 2 of the GNU General Public License as
  | published by the Free Software Foundation.
  |
  | This program is distributed in the hope that it will be useful,
  | but WITHOUT ANY WARRANTY; without even the implied warranty of
  | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
  | GNU General Public License for more details.
  |
  | You should have received a copy of the GNU General Public License
  | along with this program; if not, contact Novell, Inc.
  |
  | To contact Novell about this file by physical or electronic mail,
  | you may find current contact information at www.novell.com
  |
  |*************************************************************************** */

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

  File:	      YQRadioButtonGroup.cc

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/


#include <QRadioButton>
#define YUILogComponent "qt-ui"
#include "YUILog.h"

#include "YQRadioButtonGroup.h"
#include "YQRadioButton.h"
#include "qstring.h"


YQRadioButtonGroup::YQRadioButtonGroup( YWidget * parent )
    : QWidget( (QWidget *) parent->widgetRep() )
    , YRadioButtonGroup( parent )
{
    setWidgetRep( this );
}


YQRadioButtonGroup::~YQRadioButtonGroup()
{
    // NOP
}


void
YQRadioButtonGroup::addRadioButton( YRadioButton * button )
{
    YRadioButtonGroup::addRadioButton( button );

    if ( button->value() )		// if this new button is active
    {
	uncheckOtherButtons( button );	// make it the only active one
    }
}


void
YQRadioButtonGroup::setEnabled( bool enabled )
{
    QWidget::setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


void
YQRadioButtonGroup::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
    YRadioButtonGroup::setSize( newWidth, newHeight );
}


#include "YQRadioButtonGroup.moc"
